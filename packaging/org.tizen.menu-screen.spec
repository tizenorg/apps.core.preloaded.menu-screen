Name:       org.tizen.menu-screen
Summary:    Menu screen
Version:    1.1.1
Release:    1.1
Group:      Applications/Core Application
License:    Flora
Source0:    %{name}-%{version}.tar.gz
Source1001: 	%{name}.manifest
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
BuildRequires: pkgconfig(shortcut)
BuildRequires: pkgconfig(sysman)
BuildRequires: pkgconfig(syspopup-caller)
BuildRequires: cmake
BuildRequires: edje-tools
BuildRequires: gettext-tools


%description
Tizen 2.x Reference Application.
User application for launching apps.

%prep
%setup -q
cp %{SOURCE1001} .

%build
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}/apps/%{name}/
make %{?jobs:-j%jobs}

%install
%make_install
mkdir -p %{buildroot}/%{_datarootdir}/license

%post
INHOUSE_ID="5000"

init_vconf()
{
	vconftool set -t int memory/idle-screen/top 0 -i -u 5000 -f
	vconftool set -t string file/private/org.tizen.menu-screen/engine "gl" -i -u 5000 -f
	vconftool set -t string db/setting/menuscreen/package_name "org.tizen.menu-screen" -i -u 5000 -f
}
init_vconf

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

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
/opt/usr/apps/%{name}/*
%{_prefix}/apps/%{name}/*
%{_datarootdir}/packages/%{name}.xml
%{_datarootdir}/license/%{name}
