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
License:    Apache-2.0
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
BuildRequires:  pkgconfig(capi-appfw-preference)
BuildRequires:  pkgconfig(capi-system-info)
BuildRequires:  pkgconfig(capi-system-system-settings)
BuildRequires:  cmake
BuildRequires:  edje-tools
BuildRequires:  gettext-tools
BuildRequires:  pkgconfig(libtzplatform-config)
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

mkdir -p %{buildroot}/usr/share/license
cp -f LICENSE %{buildroot}/usr/share/license/%{_package_name}

%post

PRIVATE_OPTION="-s org.tizen.menu-screen"

INHOUSE_ID="5000"

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{TZ_SYS_RO_APP}/org.tizen.menu-screen/bin/*
%{TZ_SYS_RO_APP}/org.tizen.menu-screen/res/edje/*
%{TZ_SYS_RO_APP}/org.tizen.menu-screen/res/images/*
%{TZ_SYS_RO_APP}/org.tizen.menu-screen/res/data/main_operation.launch
%{TZ_SYS_RO_APP}/org.tizen.menu-screen/res/locale/*
%{TZ_SYS_RO_APP}/org.tizen.menu-screen/shared/res/org.tizen.menu-screen.png
#%{TZ_SYS_RO_PACKAGES}/org.tizen.menu-screen.xml
/usr/share/packages/org.tizen.menu-screen.xml
/usr/share/license/%{_package_name}
