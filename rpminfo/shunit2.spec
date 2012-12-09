Name: shunit2
Version: 2.1.5
Release: 0
Group: Development/Tools
Source: %{name}-%{version}.tgz
Patch0: shunit2-mm.patch
License: LGPL
URL: http://code.google.com/p/shunit2/
Vendor: Forest Enterprises
Summary: Unit testing framework for shell scripts.
Buildroot: %{_tmppath}/%{name}-root
BuildArch: noarch

%description
shUnit2 is a xUnit unit test framework for Bourne based shell scripts, and it is designed to work in a similar manner to JUnit, PyUnit, etc. If you have ever had the desire to write a unit test for a shell script, shUnit2 can do the job. 

%prep
%setup
%patch0 -p0

%build
make DESTDIR=$RPM_BUILD_ROOT

%install
install -d %{buildroot}/usr/bin
install build/shunit2_collect %{buildroot}/usr/bin
install build/shunit2_testsuite %{buildroot}/usr/bin

install -d %{buildroot}/usr/lib/shunit2
install build/shunit2 %{buildroot}/usr/lib/shunit2

install -d %{buildroot}/usr/share/doc/shunit2
install build/shunit2.html %{buildroot}/usr/share/doc/shunit2

%clean
rm -fr %{buildroot}

%files
%defattr(755, root, root)
/usr/bin/shunit2_collect
/usr/bin/shunit2_testsuite
/usr/lib/shunit2/shunit2
%attr(644, root, root)/usr/share/doc/shunit2/shunit2.html

%changelog
* Thu Oct 08 2009 moulinfr@gmail.com
- Initial release.
