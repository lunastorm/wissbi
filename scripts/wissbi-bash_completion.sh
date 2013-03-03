# bash completion for wissbi

_wissbi ()
{
    local cur=${COMP_WORDS[COMP_CWORD]}
    local dir=$(echo $cur | sed -e '/\//!d ; s/\(.*\)\/.*/\1\//')
    local last=$(echo $cur | sed -e 's/.*\///')
    COMPREPLY=($(for dest in $(compgen -W "`cd /var/lib/wissbi/sub/$dir ; ls | grep -v ','`" -- $last); do echo $dir$dest/ ; done))
    compopt -o nospace
}
complete -F _wissbi wissbi-sub
complete -F _wissbi wissbi-pub

# Local variables:
# mode: shell-script
# sh-basic-offset: 4
# sh-indent-comment: t
# indent-tabs-mode: nil
# End:
# ex: ts=4 sw=4 et filetype=sh
