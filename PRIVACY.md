# Privacy & Crash Reporting

CoopAndreas may automatically send anonymous crash reports when an error occurs to help us fix bugs and improve stability.

## What Is Sent
Technical debugging data only, including:
* Game and CoopAndreas versions
* Error/exception codes
* Memory state (CPU registers, stack dump, backtrace)
* Loaded modules and active scripts

## What Is NOT Sent
We do **not** collect personal data such as:
* Passwords or usernames
* Personal documents or browser history
* Save files

### Path Anonymization
To protect your privacy, local folder paths are masked before sending:
* `C:\Users\<username>` → `%USER%`
* `C:\Windows` → `%WINDIR%`
* Game installation folder → `%GAME%`

Crash report data is used strictly for debugging purposes.

## How to Disable Crash Reporting
You can turn off crash reports at any time by editing your configuration file:

1. Open `%USER%\Documents\GTA San Andreas User Files\coopandreas.ini`
2. Set `report-crashlogs` to `false`

## Example data sent to our servers

```
GTA SA 1.0.0.0 US, CoopAndreas v0.3.0-alpha Aug  5 2026 22:04:33
Unhandled exception at 0x6D9E45C9 in CoopAndreasSA.dll (+0x001145C9)
0xC0000005: Access violation write

Register dump:
   EAX: 0x00001388     EBX: 0x00000000     ECX: 0xFFFF8000     EDX: 0x00000000  
   ESI: 0x0177FC1C     EDI: 0x0000001A     EBP: 0x0177FA64     ESP: 0x0177F948  
   EIP: 0x6D9E45C9     EFL: 0x00210246  
   CS:  0x00000023     SS:  0x0000002B  
   GS:  0x0000002B     FS:  0x00000053  
   ES:  0x0000002B     DS:  0x0000002B  

Stack dump:
   0x0177F948:  482133DA 0177FAB4 00000000 00000000 00000000 0000019C
   0x0177F960:  17B40001 17B48938 00000021 000007FF 000007FF 000007FF
   0x0177F978:  00000001 0000FFFF 135812A4 17B4001A 00000064 08182E26
   0x0177F990:  135812A4 00000000 00000000 000007FF 00000000 00D7FE00
   0x0177F9A8:  08182E84 007900C0 13581260 17B5A958 00000000 010116F0
   0x0177F9C0:  00001948 45121467 C4C9DDFA 40A810CC C4D0675D C0DA0000
   0x0177F9D8:  00000001 0000000A 00000000 006E6CCF 0000000A 00000002
   0x0177F9F0:  0000000B 00000000 41300000 0072D579 0177FA74 0072D581
   0x0177FA08:  0000001A 0177FC1C 00000000 0000000B 00000008 BEFBADD3
   0x0177FA20:  3F07F6F4 41072510 00000005 413B09F4 00000003 00000003
   0x0177FA38:  00000003 00000002 0000000B 40254162 0177FA58 6D9CFD51
   0x0177FA50:  019B5424 482133DA 0177FA9C 6DDB47CB FFFFFFFF 0177FA6C
   0x0177FA68:  6D9D984B 0177FA7C 6D9E8A53 0177FAB4 0177FAB0 0177FA8C
   0x0177FA80:  6D94BEB6 0177FAB0 0177FAB0 0177FAA8 6D940605 48213316
   0x0177FA98:  0177FAEB 0177FB04 6DDAB97D 00000000 0177FAEC 6D9E484E
   0x0177FAB0:  6DE76E8C 019B5480 6DF9E5C8 0177FAFC 0177FADC 6D95218D
   0x0177FAC8:  019B5480 6DF9E5C8 0177FB00 0177FAB0 0177FB8B 0177FAB0
   0x0177FAE0:  0177FB8B 0177FB34 6D94AC0F 0177FB10 6D9D8A0B 019B5420
   0x0177FAF8:  482132AE 019B5480 019B5418 0177FB8C 6DDB31B5 00000001
   0x0177FB10:  0177FB98 6D9E1881 0177FB3C 019BD790 0177FB28 019B5418
   0x0177FB28:  019BD790 00000000 019B5480 0177FB8B 48213226 0177FC20
   0x0177FB40:  0177FB1C 0177FB28 0177FB8B 0177FB8B 0177FB1C 0177FB28
   0x0177FB58:  0177FB8B 00000000 00000001 6DF9E5C8 6DF9E5C8 0177FB8B
   0x0177FB70:  0177FB8B 6DF9E5BC 6DF9E5BC 0177FB8B 0177FB8B 019B50EC
   0x0177FB88:  6D947674 0177FC08 6DDB43CD 00000000 0177FBD0 6D9D95EF
   0x0177FBA0:  6DE77234 019B1C28 0177FBB4 6DC7E98D 0177FBC0 6D9D1771
   0x0177FBB8:  0177FD38 0177FC1C 0177FD34 0177FBA0 0177FBA0 0177FD34
   0x0177FBD0:  0177FBE4 6D9E87A0 019B50EC 0177FC1C 019B50E8 0177FC14
   0x0177FBE8:  6D94BE33 0177FC1C 482135AA 6D8DF326 6D8E43AD 0177FC1C
   0x0177FC00:  019B50E8 019B50E8 0177FD60 6DDACF3D 00000000 0177FD6C
   base: 0x01580000   top: 0x0177F948   bottom: 0x01780000

Backtrace:
   0x6D9E3E70: <lambda_ad5e642024648942714ee02ce26e22da>::operator() in gta_sa.exe (+0x6d9e3e70) (0x6D9E45C9)
   0x6D9D9840: std::invoke<<lambda_ad5e642024648942714ee02ce26e22da> &> in CoopAndreasSA.dll (+0x109840) (0x6D9D984B)
   0x6D9E8A40: std::_Func_impl_no_alloc<<lambda_ad5e642024648942714ee02ce26e22da>,void>::_Do_call in CoopAndreasSA.dll (+0x118a40) (0x6D9E8A53)
   0x6D94BE80: std::_Func_class<void>::operator() in CoopAndreasSA.dll (+0x7be80) (0x6D94BEB6)
   0x6D9405D0: plugin::ArgPick<plugin::ArgTypes<> >::operator()<std::tuple<> > in CoopAndreasSA.dll (+0x705d0) (0x6D940605)
   0x6D9E4820: <lambda_bfa158a016955d9f091fab5039ce24c3>::operator() in CoopAndreasSA.dll (+0x114820) (0x6D9E484E)
   0x6D9D8980: std::for_each<std::_List_iterator<std::_List_val<std::_List_simple_types<std::pair<std::function<void __cdecl(void)>,unsigned int> > > >,<lambda_bfa158a016955d9f091fab5039ce24c3> > in CoopAndreasSA.dll (+0x108980) (0x6D9D8A0B)
   0x6D9E1780: <lambda_37344866363e7d1865b1bf6a4f433df7>::operator() in CoopAndreasSA.dll (+0x111780) (0x6D9E1881)
   0x6D9D95D0: std::invoke<<lambda_37344866363e7d1865b1bf6a4f433df7> &,std::function<void * __cdecl(void)> > in CoopAndreasSA.dll (+0x1095d0) (0x6D9D95EF)
   0x6D9E8780: std::_Func_impl_no_alloc<<lambda_37344866363e7d1865b1bf6a4f433df7>,void *,std::function<void * __cdecl(void)> >::_Do_call in CoopAndreasSA.dll (+0x118780) (0x6D9E87A0)
   0x6D94BDC0: std::_Func_class<void *,std::function<void * __cdecl(void)> >::operator() in CoopAndreasSA.dll (+0x7bdc0) (0x6D94BE33)
   0x6D9EC8E0: injector::function_hooker_manager<injector::scoped_call,injector::function_hooker_base<injector::scoped_call,5499265,void * (__cdecl*)(void),void *>,void *>::call_hooks in CoopAndreasSA.dll (+0x11c8e0) (0x6D9ECA0A)
   0x6D9EC1B0: injector::function_hooker_base<injector::scoped_call,5499265,void * (__cdecl*)(void),void *>::call_hooks in CoopAndreasSA.dll (+0x11c1b0) (0x6D9EC1B8)
   0x6D9EC160: injector::function_hooker<injector::scoped_call,5499265,void * __cdecl(void)>::call in CoopAndreasSA.dll (+0x11c160) (0x6D9EC168)
   0x0053E920: Z4IdlePv in CoopAndreasSA.dll (+0x92c6e920) (0x0053E986)
   0x0082413F: free in gta_sa.exe (+0x42413f) (0x008241AF)
   0x004D8FA0: ZN16CAEAudioHardware11GetBeatInfoEP9tBeatInfo in gta_sa.exe (+0xd8fa0) (0x004D9486)
   0x0053EC10: AppEventHandler in gta_sa.exe (+0x13ec10) (0x0053ECC2)
   0x00619B60: RsEventHandler in gta_sa.exe (+0x219b60) (0x00619B71)
   0x00748710: WinMain in gta_sa.exe (+0x348710) (0x00748DA0)
   0x00824570: _CrtMain in gta_sa.exe (+0x424570) (0x008246F1)
   0x00824570: _CrtMain in gta_sa.exe (+0x424570) (0x00824588)


Loaded modules:
   Base: 0x00400000  Module: %GAME%\gta_sa.exe  Version: N/A
   Base: 0x77390000  Module: %WINDIR%\SYSTEM32\ntdll.dll  Version: 6.2.19041.6456
   Base: 0x76370000  Module: %WINDIR%\System32\KERNEL32.DLL  Version: 6.2.19041.6456
   Base: 0x764C0000  Module: %WINDIR%\System32\KERNELBASE.dll  Version: 6.2.19041.6456
   Base: 0x71D30000  Module: %WINDIR%\SYSTEM32\apphelp.dll  Version: 6.2.19041.6157
   Base: 0x71800000  Module: %WINDIR%\SYSTEM32\AcLayers.DLL  Version: 6.2.19041.5438
   Base: 0x756B0000  Module: %WINDIR%\System32\msvcrt.dll  Version: 7.0.19041.3636
   Base: 0x75E30000  Module: %WINDIR%\System32\USER32.dll  Version: 6.2.19041.6456
   Base: 0x77360000  Module: %WINDIR%\System32\win32u.dll  Version: 6.2.19041.6456
   Base: 0x75C70000  Module: %WINDIR%\System32\GDI32.dll  Version: 6.2.19041.6157
   Base: 0x76B50000  Module: %WINDIR%\System32\gdi32full.dll  Version: 6.2.19041.6456
   Base: 0x762F0000  Module: %WINDIR%\System32\msvcp_win.dll  Version: 6.2.19041.3636
   Base: 0x767F0000  Module: %WINDIR%\System32\ucrtbase.dll  Version: 6.2.19041.3636
   Base: 0x76D40000  Module: %WINDIR%\System32\SHELL32.dll  Version: 6.2.19041.6456
   Base: 0x76460000  Module: %WINDIR%\System32\SHLWAPI.dll  Version: 6.2.19041.6392
   Base: 0x76C40000  Module: %WINDIR%\System32\OLEAUT32.dll  Version: 6.2.19041.3636
   Base: 0x759F0000  Module: %WINDIR%\System32\combase.dll  Version: 6.2.19041.6456
   Base: 0x75D70000  Module: %WINDIR%\System32\RPCRT4.dll  Version: 6.2.19041.6392
   Base: 0x75200000  Module: %WINDIR%\System32\SETUPAPI.dll  Version: 6.2.19041.5794
   Base: 0x77320000  Module: %WINDIR%\System32\cfgmgr32.dll  Version: 6.2.19041.3996
   Base: 0x75770000  Module: %WINDIR%\System32\bcrypt.dll  Version: 6.2.19041.5438
   Base: 0x73B60000  Module: %WINDIR%\SYSTEM32\MPR.dll  Version: 6.2.19041.3636
   Base: 0x66680000  Module: %WINDIR%\SYSTEM32\sfc.dll  Version: 6.2.19041.4842
   Base: 0x71CB0000  Module: %WINDIR%\SYSTEM32\WINSPOOL.DRV  Version: 6.2.19041.5794
   Base: 0x76160000  Module: %WINDIR%\System32\shcore.dll  Version: 6.2.19041.5794
   Base: 0x720E0000  Module: %WINDIR%\SYSTEM32\sfc_os.DLL  Version: 6.2.19041.4842
   Base: 0x713D0000  Module: %WINDIR%\SYSTEM32\AcGenral.DLL  Version: 6.2.19041.3636
   Base: 0x76270000  Module: %WINDIR%\System32\sechost.dll  Version: 6.2.19041.6328
   Base: 0x75FD0000  Module: %WINDIR%\System32\ole32.dll  Version: 6.2.19041.5915
   Base: 0x76710000  Module: %WINDIR%\System32\ADVAPI32.dll  Version: 6.2.19041.6328
   Base: 0x71630000  Module: %WINDIR%\SYSTEM32\UxTheme.dll  Version: 6.2.19041.6392
   Base: 0x73B80000  Module: %WINDIR%\SYSTEM32\WINMM.dll  Version: 6.2.19041.3636
   Base: 0x720B0000  Module: %WINDIR%\SYSTEM32\samcli.dll  Version: 6.2.19041.3636
   Base: 0x71C90000  Module: %WINDIR%\SYSTEM32\MSACM32.dll  Version: 6.2.19041.1
   Base: 0x723B0000  Module: %WINDIR%\SYSTEM32\VERSION.dll  Version: 6.2.19041.3636
   Base: 0x751A0000  Module: %WINDIR%\SYSTEM32\USERENV.dll  Version: 6.2.19041.5794
   Base: 0x70A40000  Module: %WINDIR%\SYSTEM32\dwmapi.dll  Version: 6.2.19041.6157
   Base: 0x71220000  Module: %WINDIR%\SYSTEM32\urlmon.dll  Version: 11.0.19041.6456
   Base: 0x727B0000  Module: %WINDIR%\SYSTEM32\SspiCli.dll  Version: 6.2.19041.6328
   Base: 0x71C70000  Module: %WINDIR%\SYSTEM32\winmmbase.dll  Version: 6.2.19041.1
   Base: 0x6E020000  Module: %WINDIR%\SYSTEM32\iertutil.dll  Version: 11.0.19041.6456
   Base: 0x71C30000  Module: %WINDIR%\SYSTEM32\srvcli.dll  Version: 6.2.19041.3636
   Base: 0x72CA0000  Module: %WINDIR%\SYSTEM32\netutils.dll  Version: 6.2.19041.3636
   Base: 0x73100000  Module: %WINDIR%\SYSTEM32\windows.storage.dll  Version: 6.2.19041.6456
   Base: 0x73060000  Module: %WINDIR%\SYSTEM32\Wldp.dll  Version: 6.2.19041.5794
   Base: 0x760D0000  Module: %WINDIR%\System32\IMM32.DLL  Version: 6.2.19041.6157
   Base: 0x75640000  Module: %WINDIR%\System32\WS2_32.dll  Version: 6.2.19041.3636
   Base: 0x71710000  Module: %GAME%\EAX.DLL  Version: N/A
   Base: 0x001F0000  Module: %GAME%\vorbisfile.dll  Version: N/A
   Base: 0x10000000  Module: %GAME%\eax_orig.DLL  Version: 4.0.0.1
   Base: 0x01840000  Module: %GAME%\vorbishooked.dll  Version: N/A
   Base: 0x01860000  Module: %GAME%\ogg.dll  Version: N/A
   Base: 0x01AF0000  Module: %GAME%\vorbis.dll  Version: N/A
   Base: 0x711A0000  Module: %GAME%\CLEO.asi  Version: 4.4.4.0
   Base: 0x716C0000  Module: %GAME%\bass.dll  Version: 2.4.18.3
   Base: 0x71180000  Module: %GAME%\cleo\FileSystemOperations.cleo  Version: N/A
   Base: 0x71160000  Module: %GAME%\cleo\IniFiles.cleo  Version: N/A
   Base: 0x71140000  Module: %GAME%\cleo\IntOperations.cleo  Version: N/A
   Base: 0x70E70000  Module: %GAME%\GTASA.WidescreenFix.asi  Version: 1.2.0.0
   Base: 0x70E20000  Module: %GAME%\III.VC.SA.WindowedMode.asi  Version: 1.2.0.0
   Base: 0x71110000  Module: %GAME%\scrlog.asi  Version: N/A
   Base: 0x720A0000  Module: %GAME%\SkipDeviceSelection.asi  Version: N/A
   Base: 0x751D0000  Module: %WINDIR%\SYSTEM32\VCRUNTIME140.dll  Version: 14.44.35211.0
   Base: 0x6D8D0000  Module: %GAME%\CoopAndreasSA.dll  Version: 0.3.0.0
   Base: 0x73720000  Module: %WINDIR%\SYSTEM32\dbghelp.dll  Version: 6.2.19041.5848
   Base: 0x6CEE0000  Module: %WINDIR%\SYSTEM32\d3dx9_43.dll  Version: 9.29.952.3111
   Base: 0x73A90000  Module: %WINDIR%\SYSTEM32\WINHTTP.dll  Version: 6.2.19041.5794
   Base: 0x730D0000  Module: %WINDIR%\SYSTEM32\dbgcore.DLL  Version: 6.2.19041.5794
   Base: 0x73000000  Module: %WINDIR%\SYSTEM32\kernel.appcore.dll  Version: 6.2.19041.3758
   Base: 0x6CDF0000  Module: %WINDIR%\SYSTEM32\DDRAW.dll  Version: 6.2.19041.5794
   Base: 0x6E8B0000  Module: %WINDIR%\SYSTEM32\dxgi.dll  Version: 6.2.19041.5794
   Base: 0x70E10000  Module: %WINDIR%\SYSTEM32\DCIMAN32.dll  Version: 6.2.19041.5794
   Base: 0x6D8A0000  Module: %WINDIR%\SYSTEM32\dxcore.dll  Version: 6.2.19041.5794
   Base: 0x76100000  Module: %WINDIR%\System32\bcryptPrimitives.dll  Version: 6.2.19041.5438
   Base: 0x6CD70000  Module: %WINDIR%\SYSTEM32\DSOUND.dll  Version: 6.2.19041.4355
   Base: 0x6EBC0000  Module: %WINDIR%\SYSTEM32\powrprof.dll  Version: 6.2.19041.3636
   Base: 0x6EBB0000  Module: %WINDIR%\SYSTEM32\UMPDC.dll  Version: N/A
   Base: 0x761F0000  Module: %WINDIR%\System32\clbcatq.dll  Version: 2001.12.10941.16384
   Base: 0x6CD00000  Module: %WINDIR%\System32\MMDevApi.dll  Version: 6.2.19041.5915
   Base: 0x739E0000  Module: %WINDIR%\System32\DEVOBJ.dll  Version: 6.2.19041.5794
   Base: 0x6CBC0000  Module: %WINDIR%\SYSTEM32\AUDIOSES.DLL  Version: 6.2.19041.5794
   Base: 0x70E00000  Module: %WINDIR%\SYSTEM32\resourcepolicyclient.dll  Version: 6.2.19041.4355
   Base: 0x76910000  Module: %WINDIR%\System32\MSCTF.dll  Version: 6.2.19041.5794
   Base: 0x6CBB0000  Module: %WINDIR%\SYSTEM32\avrt.DLL  Version: 6.2.19041.5438
   Base: 0x6CB70000  Module: %WINDIR%\SYSTEM32\DINPUT8.dll  Version: 6.2.19041.1
   Base: 0x6CA80000  Module: %WINDIR%\SYSTEM32\inputhost.dll  Version: 6.2.19041.5848
   Base: 0x70FB0000  Module: %WINDIR%\SYSTEM32\CoreMessaging.dll  Version: 6.2.19041.5915
   Base: 0x70ED0000  Module: %WINDIR%\SYSTEM32\wintypes.dll  Version: 6.2.19041.6456
   Base: 0x6E260000  Module: %WINDIR%\SYSTEM32\CoreUIComponents.dll  Version: 6.2.19041.3636
   Base: 0x6C9C0000  Module: %WINDIR%\SYSTEM32\PROPSYS.dll  Version: 7.0.19041.5794
   Base: 0x73010000  Module: %WINDIR%\SYSTEM32\ntmarta.dll  Version: 6.2.19041.3636
   Base: 0x6C9B0000  Module: %WINDIR%\SYSTEM32\HID.DLL  Version: 6.2.19041.3636
   Base: 0x76790000  Module: %WINDIR%\System32\WINTRUST.dll  Version: 6.2.19041.6456
   Base: 0x76A50000  Module: %WINDIR%\System32\CRYPT32.dll  Version: 6.2.19041.6157
   Base: 0x6C9A0000  Module: %WINDIR%\SYSTEM32\MSASN1.dll  Version: 6.2.19041.3636
   Base: 0x70A70000  Module: %WINDIR%\SYSTEM32\d3d9.dll  Version: 6.2.19041.5794
   Base: 0x70560000  Module: %WINDIR%\System32\DriverStore\FileRepository\u0415346.inf_amd64_65a03ee01ac1140e\B413472\aticfx32.dll  Version: 31.0.21923.1000
   Base: 0x70530000  Module: %WINDIR%\System32\DriverStore\FileRepository\u0415346.inf_amd64_65a03ee01ac1140e\B413472\atiu9pag.dll  Version: 31.0.21923.1000
   Base: 0x6FA70000  Module: %WINDIR%\System32\DriverStore\FileRepository\u0415346.inf_amd64_65a03ee01ac1140e\B413472\atiumdag.dll  Version: 31.0.21923.1000
   Base: 0x6EC10000  Module: %WINDIR%\System32\DriverStore\FileRepository\u0415346.inf_amd64_65a03ee01ac1140e\B413472\atiumdva.dll  Version: 31.0.21923.1000
   Base: 0x6EB80000  Module: %WINDIR%\SYSTEM32\amdihk32.dll  Version: 2.0.0.1788
   Base: 0x71DE0000  Module: %WINDIR%\WinSxS\x86_microsoft.windows.common-controls_6595b64144ccf1df_6.0.19041.6456_none_a865dda286726b5c\comctl32.DLL  Version: 6.10.19041.6456
   Base: 0x71050000  Module: %WINDIR%\SYSTEM32\textinputframework.dll  Version: 6.2.19041.5794
   Base: 0x6C990000  Module: %WINDIR%\SYSTEM32\xinput1_4.dll  Version: 6.2.19041.3636
   Base: 0x6C970000  Module: %WINDIR%\SYSTEM32\usp10.dll  Version: 6.2.19041.3636
   Base: 0x72000000  Module: %WINDIR%\SYSTEM32\TextShaping.dll  Version: N/A
   Base: 0x6C8F0000  Module: %WINDIR%\SYSTEM32\webio.dll  Version: 6.2.19041.6033
   Base: 0x73940000  Module: %WINDIR%\system32\mswsock.dll  Version: 6.2.19041.5915
   Base: 0x74A70000  Module: %WINDIR%\SYSTEM32\IPHLPAPI.DLL  Version: 6.2.19041.3636
   Base: 0x6B430000  Module: %WINDIR%\SYSTEM32\WINNSI.DLL  Version: 6.2.19041.3636
   Base: 0x760C0000  Module: %WINDIR%\System32\NSI.dll  Version: 6.2.19041.5438
   Base: 0x749E0000  Module: %WINDIR%\SYSTEM32\DNSAPI.dll  Version: 6.2.19041.5369
   Base: 0x6B3B0000  Module: %WINDIR%\System32\rasadhlp.dll  Version: 6.2.19041.3636
   Base: 0x74980000  Module: %WINDIR%\System32\fwpuclnt.dll  Version: 6.2.19041.6328
   Base: 0x6C870000  Module: %WINDIR%\System32\schannel.dll  Version: 6.2.19041.6328
   Base: 0x6C770000  Module: %WINDIR%\System32\Windows.UI.dll  Version: 6.2.19041.5794
   Base: 0x6C6F0000  Module: %WINDIR%\System32\WindowManagementAPI.dll  Version: N/A
   Base: 0x6C550000  Module: %WINDIR%\System32\twinapi.appcore.dll  Version: 6.2.19041.5848
   Base: 0x6C340000  Module: %WINDIR%\SYSTEM32\wmvcore.dll  Version: 12.0.19041.6456
   Base: 0x6C300000  Module: %WINDIR%\SYSTEM32\WMASF.DLL  Version: 12.0.19041.1
   Base: 0x6C1F0000  Module: %WINDIR%\SYSTEM32\mfperfhelper.dll  Version: 6.2.19041.1
   Base: 0x6C1B0000  Module: %WINDIR%\system32\mlang.dll  Version: 6.2.19041.3636
   Base: 0x6C1A0000  Module: %WINDIR%\SYSTEM32\mskeyprotect.dll  Version: 6.2.19041.5438
   Base: 0x6C170000  Module: %WINDIR%\SYSTEM32\NTASN1.dll  Version: 6.2.19041.3636
   Base: 0x6C140000  Module: %WINDIR%\SYSTEM32\ncrypt.dll  Version: 6.2.19041.5438
   Base: 0x6C120000  Module: %WINDIR%\system32\ncryptsslp.dll  Version: 6.2.19041.5438
   Base: 0x6C110000  Module: %WINDIR%\SYSTEM32\DPAPI.DLL  Version: 6.2.19041.3636
   Base: 0x6B410000  Module: %WINDIR%\system32\napinsp.dll  Version: 6.2.19041.3636
   Base: 0x6B3F0000  Module: %WINDIR%\system32\pnrpnsp.dll  Version: 6.2.19041.3636
   Base: 0x6B3D0000  Module: %WINDIR%\system32\wshbth.dll  Version: 6.2.19041.5848
   Base: 0x72320000  Module: %WINDIR%\system32\NLAapi.dll  Version: 6.2.19041.6456
   Base: 0x6B3C0000  Module: %WINDIR%\System32\winrnr.dll  Version: 6.2.19041.3636

Active scripts:
   introst    BIP 00000000   CIP 00A60A6B   0000
   mob_ran    BIP 00000000   CIP 00A778FD   0000
   mob_la1    BIP 00000000   CIP 00A75957   0000
   int        BIP 00000000   CIP 00A58AF1   0000
   gfagnt     BIP 00000000   CIP 00A6C1E4   0000
   intman     BIP 00000000   CIP 00A6F6D0   0000
   hj         BIP 00000000   CIP 00A7097F   0000
   apcheck    BIP 00000000   CIP 00A6ACD4   0000
   tri        BIP 00000000   CIP 00A5D6C3   0000
   openup     BIP 00000000   CIP 00A61F22   0000
   impnd_l    BIP 00000000   CIP 00A5E019   0000
   trainsl    BIP 00000000   CIP 00A5E3AE   0000
   adplane    BIP 00000000   CIP 00A5E428   0000
   valet_l    BIP 00000000   CIP 00A5E2E2   0000
   buy_pro    BIP 00000000   CIP 00A5DF61   0000
   cranes     BIP 00000000   CIP 00A61B79   0000
   colls      BIP 00000000   CIP 00A5E7F4   0000
   help       BIP 00000000   CIP 00A601D2   0000
   flow       BIP 00000000   CIP 00A60DD0   0000
   psave1     BIP 00000000   CIP 00A5EF4E   0000
   kicks      BIP 00000000   CIP 00A5D5AE   0000
   hotr       BIP 00000000   CIP 00A5D4E0   0000
   bloodr     BIP 00000000   CIP 00A5D451   0000
   shoot      BIP 00000000   CIP 00A5BE87   0000
   gym        BIP 00000000   CIP 00A5BFD2   0000
   r3         BIP 00000000   CIP 00A5C3FA   0000
   oddveh     BIP 00000000   CIP 00A5D936   0000
   main       BIP 00000000   CIP 00A58417   0000
   No Clip    BIP 0F652C70   CIP 0F652D82   0000
   mdc        BIP 147E48A8   CIP 147E48E4   0000
   cheats     BIP 147BCF08   CIP 147BD3B4   0000
```