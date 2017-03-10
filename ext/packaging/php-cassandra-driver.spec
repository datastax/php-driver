%define distnum %(/usr/lib/rpm/redhat/dist.sh --distnum)

%if "%{php_version}" < "5.6"
%global ini_name   cassandra.ini
%else
%global ini_name   40-cassandra.ini
%endif

Name:    %{package_name}
Epoch:   1
Version: %{driver_version}
Release: 1%{?dist}
Summary: DataStax PHP DataStax Enterprise Driver

Group: Development/Tools
License: Proprietary
URL: https://github.com/datastax/php-driver
Source0: %{name}-%{version}.tar.gz

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
%if %{distnum} == 5
BuildRequires: buildsys-macros >= 5
%endif
BuildRequires: cassandra-cpp-driver-devel >= 2.6.0
BuildRequires: php-devel >= 5.6
BuildRequires: gmp-devel

Requires: php(zend-abi) = %{php_zend_api}
Requires: php(api) = %{php_core_api}
Requires: cassandra-cpp-driver >= 2.6.0
Requires: gmp

%description
A modern, feature-rich and highly tunable PHP client library for Apache
Cassandra (1.2+) and DataStax Enterprise (3.1+) using exclusively Cassandra's
binary protocol and Cassandra Query Language v3.

%prep
%setup -qn %{name}-%{version}

cat << 'EOF' | tee  %{ini_name}
; Enable %{summary} extension module
extension=cassandra.so
EOF

%build
%{_bindir}/phpize
export CFLAGS=-Wno-extended-offsetof
%configure \
    --with-cassandra \
    --with-libdir=%{_lib} \
    --with-php-config=%{_bindir}/php-config
make %{?_smp_mflags}

%install
make install INSTALL_ROOT=%{buildroot}
install -D -m 644 %{ini_name} %{buildroot}%{php_inidir}/%{ini_name}

%clean
rm -rf %{buildroot}

%check
# make check

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%config(noreplace) %{php_inidir}/%{ini_name}
%{php_extdir}/cassandra.so

%changelog
* Mon Mar 13 2017 Michael Penick <michael.penick@datastax.com> - 1.3.0-1
- release
