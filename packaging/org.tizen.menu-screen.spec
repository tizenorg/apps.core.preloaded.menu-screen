%define _optdir /opt
%define _usrdir /usr
%define _appdir %{_usrdir}/apps
%define _usr_datadir %{_usrdir}/share

%define _project_name menu-screen
%define _package_name org.tizen.%{_project_name}

%define _packagedir %{_appdir}/%{_package_name}
%define _appsbindir %{TZ_SYS_RO_APP}/%{_package_name}/bin
%define _optdatadir %{_optdir}%{_packagedir}/data
%define _resdir %{TZ_SYS_RO_APP}/%{_package_name}/res
%define _sharedir %{_packagedir}/share

%define _localedir %{_resdir}/locale

Name:       org.tizen.menu-screen
Summary:    An utility library of the menu screen
Version:    1.1.1
Release:    1.1
Group:      TO_BE/FILLED_IN
License:    Flora Software License
Source0:    %{name}-%{version}.tar.gz
Source1001: 	org.tizen.menu-screen.manifest
Source1002: 	init_menu-screen_DB.sh
BuildRequires:  pkgconfig(ail)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(appsvc)
BuildRequires:  pkgconfig(aul)
BuildRequires:  pkgconfig(badge)
BuildRequires:  pkgconfig(bundle)
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(capi-system-info)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(ecore-evas)
BuildRequires:  pkgconfig(ecore-file)
BuildRequires:  pkgconfig(ecore-imf)
BuildRequires:  pkgconfig(ecore-input)
BuildRequires:  pkgconfig(edje)
BuildRequires:  pkgconfig(eet)
BuildRequires:  pkgconfig(eina)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(heynoti)
BuildRequires:  pkgconfig(pkgmgr)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(shortcut)
BuildRequires:  pkgconfig(sysman)
BuildRequires:  pkgconfig(syspopup-caller)
BuildRequires:  cmake
BuildRequires:  edje-tools
BuildRequires:  gettext-tools
BuildRequires:  pkgconfig(libtzplatform-config)

%description
An utility library for developers of the menu screen.



%package devel
Summary:    An utility library of the menu screen (devel)
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
An utility library for developers of the menu screen (devel)


%prep
%setup -q
cp %{SOURCE1001} .

%build
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} \
-DTZ_SYS_ETC=%TZ_SYS_ETC \
-DTZ_SYS_RO_PACKAGES=%TZ_SYS_RO_PACKAGES \
-DTZ_SYS_RO_APP=%TZ_SYS_RO_APP \
-DTZ_SYS_RW_APP=%TZ_SYS_RW_APP \
-DTZ_SYS_RW_PACKAGES=%TZ_SYS_RW_PACKAGES \
-DTZ_SYS_USER_GROUP=%TZ_SYS_USER_GROUP
CFLAGS="${CFLAGS} -Wall -Werror" LDFLAGS="${LDFLAGS} -Wl,--hash-style=both -Wl,--as-needed"
make %{?jobs:-j%jobs}

%install
%make_install
mkdir -p %{buildroot}%{_optdatadir}
mkdir -p %{buildroot}/usr/share/license
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
%{_appsbindir}/menu-screen
%{_resdir}/edje/all_apps_portrait.edj
%{_resdir}/edje/button_edit.edj
%{_resdir}/edje/group_4x4_portrait.edj
%{_resdir}/edje/item_4x4.edj
%{_resdir}/edje/layout_portrait.edj
%{_resdir}/edje/index.edj
%{_resdir}/images/default.png
%{_localedir}/*/*/*.mo
%{_usr_datadir}/packages/org.tizen.menu-screen.xml
%{_usr_datadir}/license/%{name}
%{TZ_SYS_ETC}/smack/accesses.d/%{_package_name}.rule
%{_datadir}/%{name}/init_menu-screen_DB.sh
