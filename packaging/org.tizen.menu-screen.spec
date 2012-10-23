%define _optdir /opt
%define _appdir %{_optdir}/apps
%define _opt_datadir %{_optdir}/share

Name:       org.tizen.menu-screen
Summary:    An utility library of the menu screen
Version:    1.0.16
Release:    1.1
Group:      TO_BE/FILLED_IN
License:    Flora Software License
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(utilX)
BuildRequires:  pkgconfig(aul)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(sysman)
BuildRequires:  pkgconfig(bundle)
BuildRequires:  pkgconfig(pkgmgr)
BuildRequires:  pkgconfig(syspopup-caller)
BuildRequires:  pkgconfig(heynoti)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(ecore-evas)
BuildRequires:  pkgconfig(edje)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(ecore-file)
BuildRequires:  pkgconfig(ecore-imf)
BuildRequires:  pkgconfig(ecore-input)
BuildRequires:  pkgconfig(eet)
BuildRequires:  pkgconfig(eina)
BuildRequires:  pkgconfig(ail)
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  cmake
BuildRequires:  edje-tools
BuildRequires:  gettext-tools


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

%build
CFLAGS="-I/usr/lib/glib-2.0/include/ -I/usr/include/glib-2.0 -I/usr/lib/dbus-1.0/include -I/usr/include/dbus-1.0 -I/usr/include/e_dbus-1 -I/usr/include/ethumb-0 -I/usr/include/edje-1 -I/usr/include/efreet-1 -I/usr/include/embryo-1 -I/usr/include/ecore-1 -I/usr/include/eet-1 -I/usr/include/evas-1 -I/usr/include/eina-1 -I/usr/include/eina-1/eina $CFLAGS" CXXFLAGS=$CXXFLAGS cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}

make %{?jobs:-j%jobs}

%install
%make_install


%post

INHOUSE_ID="5000"

init_vconf()
{
	# for menu daemon
	vconftool set -t int memory/menu-screen/is_menu_screen_done 0 -i -f

	# for menu-screen
	vconftool set -t int memory/idle-screen/top 0 -i -u 5000 -f
	vconftool set -t string db/setting/menuscreen/package_name "org.tizen.menu-screen" -i -u 5000 -f
}



init_vconf

%files 
%manifest %{name}.manifest
%{_appdir}/org.tizen.menu-screen/bin/menu-screen
%{_appdir}/org.tizen.menu-screen/res/edje/all_apps_portrait.edj
%{_appdir}/org.tizen.menu-screen/res/edje/group_4x4_portrait.edj
%{_appdir}/org.tizen.menu-screen/res/edje/item_4x4.edj
%{_appdir}/org.tizen.menu-screen/res/edje/layout_portrait.edj
%{_appdir}/org.tizen.menu-screen/res/edje/index.edj
%{_opt_datadir}/packages/org.tizen.menu-screen.xml


