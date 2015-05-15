%define _optdir /opt
%define _usrdir /usr
%define _appdir %{_usrdir}/apps
%define _usr_datadir %{_usrdir}/share

%define _project_name menu-screen
%define _package_name org.tizen.%{_project_name}

%define _packagedir %{_appdir}/%{_package_name}
%define _bindir %{_packagedir}/bin
%define _datadir %{_optdir}%{_packagedir}/data
%define _resdir %{_packagedir}/res
%define _sharedir %{_packagedir}/share

%define _localedir %{_resdir}/locale

%if "%{?profile}" == "wearable"
ExcludeArch: %{arm} %ix86 x86_64
%endif

Name:       org.tizen.menu-screen
Summary:    An utility library of the menu screen
Version:    1.2.5
Release:    1.1
Group:      TO_BE/FILLED_IN
License:    Apache
Source0:    %{name}-%{version}.tar.gz
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
#BuildRequires:  pkgconfig(ecore-x)
BuildRequires:  pkgconfig(ecore-evas)
BuildRequires:  pkgconfig(ecore-file)
BuildRequires:  pkgconfig(ecore-imf)
BuildRequires:  pkgconfig(ecore-input)
BuildRequires:  pkgconfig(edje)
BuildRequires:  pkgconfig(eet)
BuildRequires:  pkgconfig(eina)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(pkgmgr)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(shortcut)
BuildRequires:  pkgconfig(syspopup-caller)
#BuildRequires:  pkgconfig(utilX)
#BuildRequires:  pkgconfig(capi-appfw-preference)
BuildRequires:  pkgconfig(capi-system-info)
BuildRequires:  pkgconfig(capi-system-system-settings)
BuildRequires:  cmake
BuildRequires:  edje-tools
BuildRequires:  gettext-tools
# %ifarch %{arm}
# %define ARCH arm
# BuildRequires:  pkgconfig(bincfg)
# BuildRequires:  pkgconfig(factory)
# %else
# %define ARCH emulator
# %endif

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
CFLAGS+=" -fPIC -fPIE ";export CFLAGS
CXXFLAGS+=" -fPIC -fPIE -fvisibility=hidden -fvisibility-inlines-hidden ";export CXXFLAGS
LDFLAGS+=" -Wl,--hash-style=both -Wl,--as-needed -pie ";export LDFLAGS

%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS="${CFLAGS} -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="${CXXFLAGS} -DTIZEN_ENGINEER_MODE"
export FFLAGS="${FFLAGS} -DTIZEN_ENGINEER_MODE"
%endif

cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DARCH=%{ARCH}
make %{?jobs:-j%jobs}

%install
%make_install
%define tizen_sign 1
%define tizen_sign_base %{_packagedir}
%define tizen_sign_level platform
%define tizen_author_sign 1
%define tizen_dist_sign 1

mkdir -p %{buildroot}%{_datadir}
mkdir -p %{buildroot}/usr/share/license

%post

PRIVATE_OPTION="-s org.tizen.menu-screen"

if [ ! -d %{_datadir}/dbspace ]
then
	mkdir -p %{_datadir}/dbspace
fi

if [ ! -d %{_datadir}/shortcut ]
then
	mkdir -p %{_datadir}/shortcut
else
	rm -rf %{_datadir}/shortcut/*
fi

sqlite3 %{_datadir}/dbspace/.menu_screen.db 'PRAGMA journal_mode = PERSIST;
	create table if not exists shortcut (
		ROWID INTEGER PRIMARY KEY AUTOINCREMENT,
		appid TEXT,
		name TEXT,
		type INTEGER,
		content_info TEXT,
		icon TEXT
	);
'

INHOUSE_ID="5000"
chown -R $INHOUSE_ID:$INHOUSE_ID %{_datadir}
chown root:$INHOUSE_ID %{_datadir}/dbspace/.menu_screen.db
chown root:$INHOUSE_ID %{_datadir}/dbspace/.menu_screen.db-journal

chmod 660 %{_datadir}/dbspace/.menu_screen.db
chmod 660 %{_datadir}/dbspace/.menu_screen.db-journal

if [ -f /usr/lib/rpm-plugins/msm.so ]
then
	chsmack -a 'org.tizen.menu-screen' %{_datadir}/dbspace/.menu_screen.db*
fi

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_bindir}/menu-screen
%{_resdir}/edje/all_apps_portrait.edj
%{_resdir}/edje/button_edit.edj
%{_resdir}/edje/group_4x4_portrait.edj
%{_resdir}/edje/item_4x4.edj
%{_resdir}/edje/layout_portrait.edj
%{_resdir}/edje/index.edj
%{_resdir}/images/default.png
%{_resdir}/data/main_operation.launch
%{_localedir}/*/*/*.mo
%{_usr_datadir}/icons/default/small/org.tizen.menu-screen.png
%{_usr_datadir}/packages/org.tizen.menu-screen.xml
#%{_packagedir}/*.xml
/etc/smack/accesses.d/%{_package_name}.efl
