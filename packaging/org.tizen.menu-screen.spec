
Name: org.tizen.menu-screen
Summary: Menu screen
Version: 1.1.1
Release: 1.1
Group: Applications/Core Applications
License: Apache
Source0: %{name}-%{version}.tar.gz
Source1001: %{name}.manifest
Source1002:   init_menu-screen_DB.sh
BuildRequires: pkgconfig(ail)
BuildRequires: pkgconfig(appcore-efl)
BuildRequires: pkgconfig(appsvc)
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(badge)
BuildRequires: pkgconfig(bundle)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(capi-system-info)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(ecore-evas)
BuildRequires: pkgconfig(ecore-file)
BuildRequires: pkgconfig(ecore-imf)
BuildRequires: pkgconfig(ecore-input)
BuildRequires: pkgconfig(edje)
BuildRequires: pkgconfig(eet)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(heynoti)
BuildRequires: pkgconfig(pkgmgr)
BuildRequires: pkgconfig(pkgmgr-info)
#BuildRequires: pkgconfig(shortcut)
BuildRequires: pkgconfig(syspopup-caller)
BuildRequires: cmake
BuildRequires: edje-tools
BuildRequires: gettext-tools
BuildRequires:  pkgconfig(libtzplatform-config)

%description
Tizen 2.x Reference Application.
User application for launching apps.

%prep
%setup -q
cp %{SOURCE1001} .

%build
%cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}/apps/%{name}/  \
-DTZ_SYS_ETC=%TZ_SYS_ETC \
-DTZ_SYS_SMACK=%TZ_SYS_SMACK \
-DTZ_SYS_RO_PACKAGES=%TZ_SYS_RO_PACKAGES \
-DTZ_SYS_RO_APP=%TZ_SYS_RO_APP \
-DTZ_SYS_RW_APP=%TZ_SYS_RW_APP \
-DTZ_SYS_RW_PACKAGES=%TZ_SYS_RW_PACKAGES \
-DTZ_SYS_USER_GROUP=%TZ_SYS_USER_GROUP
make %{?jobs:-j%jobs}

%install
%make_install
mkdir -p %{buildroot}/%{_datarootdir}/license
install -D -m 0755 %{SOURCE1002} %{buildroot}%{_datadir}/%{name}/init_menu-screen_DB.sh

%post
users_gid=$(getent group %TZ_SYS_USER_GROUP | cut -f3 -d':')
INHOUSE_ID="$users_gid"

init_vconf()
{
	vconftool set -t int memory/idle-screen/top 0 -i -g $users_gid -f
	vconftool set -t string file/private/org.tizen.menu-screen/engine "gl" -i -g $users_gid -f
	vconftool set -t string db/setting/menuscreen/package_name "org.tizen.menu-screen" -i -g $users_gid -f
}
init_vconf

%{_datadir}/%{name}/init_menu-screen_DB.sh %{_datadir}

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_prefix}/apps/%{name}/*
%{_datarootdir}/packages/%{name}.xml
%{_datadir}/%{name}/init_menu-screen_DB.sh
