#ifndef WISSBI_CONNECTOR_HPP_
#define WISSBI_CONNECTOR_HPP_

#include <string>
#include <thread>
#include <unordered_set>
#include <cstdlib>
#include <list>
#include <atomic>
#include "msg_filter.hpp"
#include "sub_dir.hpp"
#include "io_policy/tee.hpp"
#include "io_policy/sysv_mq.hpp"
#include "io_policy/tcp.hpp"
#include "util.hpp"
#include "filter_metric.hpp"
#include "logger.hpp"

namespace wissbi {

template
<
    class T
>
class Connector {
    typedef std::list<FilterMetric> MetricList;
public:
    Connector(const std::string dest, T& input_filter, MetricList& metric_list, std::atomic_uint& pending_counter)
    {
        std::thread(ConnectorLoop, dest, std::ref(input_filter), std::ref(metric_list), std::ref(pending_counter)).detach();
    }

    ~Connector()
    {
    }

private:
    static void ConnectorLoop(const std::string dest, T& filter, MetricList& metric_list, std::atomic_uint& pending_counter)
    {
        std::unordered_set<std::string> producer_set;

        SubDir sub_dir(getenv("WISSBI_META_DIR") != NULL ? getenv("WISSBI_META_DIR") : "/var/lib/wissbi", dest);
        while(true) {
            for(auto sub_entry_tuple : sub_dir.GetSubList()) {
                const std::string& conn_str = std::get<0>(sub_entry_tuple);
                const std::string& real_dest = std::get<1>(sub_entry_tuple);
                if(producer_set.find(conn_str) != producer_set.end()) {
                    continue;
                }

                if(!filter.ExistsBranch(real_dest)) {
                    std::shared_ptr<io_policy::SysvMq> mq_ptr(new io_policy::SysvMq());
                    mq_ptr->mq_init(real_dest);
                    char* drop_option = getenv("WISSBI_DROP_MESSAGE");
                    if(drop_option && std::string(drop_option) == "no") {
                        mq_ptr->set_drop(false);
                    }
                    else {
                        mq_ptr->set_drop(true);
                    }
                    filter.AddBranch(real_dest, mq_ptr);
                }

                auto iter = std::find_if(metric_list.begin(), metric_list.end(), [real_dest](const FilterMetric& m){
                    return m.name == real_dest;
                });

                if(iter == metric_list.end()) {
                    metric_list.push_front(FilterMetric(real_dest));
                    iter = metric_list.begin();
                }
                FilterMetric& metric = *iter;

                std::thread([conn_str, dest, real_dest, &producer_set, &metric, &pending_counter]{
                    sockaddr sock_addr;
                    util::ConnectStringToSockaddr(conn_str, reinterpret_cast<sockaddr_in*>(&sock_addr));
                    MsgFilter<io_policy::SysvMq, io_policy::TCP> producerFilter;
                    try {
                        producerFilter.Connect(&sock_addr);
                    }
                    catch (std::exception& e) {
                        logger::log("error connecting: {}", e.what());
                        producer_set.erase(conn_str);
                        return;
                    }
                    producerFilter.set_post_filter_func([dest, real_dest, &metric, &pending_counter, &producerFilter](bool filter_result, MsgBuf& msg_buf){
                        if(filter_result == true) {
                            --pending_counter;
                            ++metric.last_processed;
                            ++metric.total_processed;
                            return true;
                        }
                        else {
                            static_cast<InputWrapper<io_policy::SysvMq>&>(producerFilter).Put(msg_buf);
                            return false;
                        }
                    });
                    producerFilter.set_cleanup(false);
                    producerFilter.mq_init(real_dest);
                    producerFilter.FilterLoop();
                    producer_set.erase(conn_str);
                }).detach();
                producer_set.insert(conn_str);

            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

}

#endif  // WISSBI_CONNECTOR_HPP_
