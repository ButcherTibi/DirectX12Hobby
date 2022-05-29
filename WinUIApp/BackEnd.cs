using System;
using System.Reflection;
using System.Diagnostics;
using System.ComponentModel;
using System.Runtime.InteropServices;


public static class Win32
{
	[DllImport("kernel32.dll")]
	public static extern IntPtr LoadLibrary(string dllToLoad);

	[DllImport("kernel32.dll")]
	public static extern bool FreeLibrary(IntPtr hModule);
}

public static class LoadTheBackend
{
	[DllImport("LoadBackend.dll")]
	public static extern void load();
}

public static class BackEnd {

	[DllImport("BackEnd.dll")]
	public static extern
	void initPixDebugger();


	/* Aplication loop is made from these methods which have a timing constraint */

	[DllImport("BackEnd.dll")]
	public static extern
	void init();

	[DllImport("BackEnd.dll")]
	public static extern
	void phase_1_runCPU();

	[DllImport("BackEnd.dll")]
	public static extern
	void phase_2_waitForRendering();

	[DllImport("BackEnd.dll")]
	public static extern unsafe
	bool phase_2X_tryDownloadRender(uint width, uint height, byte* r_pixels);

	[DllImport("BackEnd.dll")]
	public static extern
	void phase_3_render();


	/* These methods do not require timing */

	[DllImport("BackEnd.dll")]
	public static extern
	void captureFrame();


	//private static IntPtr DllImportResolver(string libraryName, Assembly assembly, DllImportSearchPath? searchPath)
	//{
	//	if (libraryName == "nativedep") {
	//		// On systems with AVX2 support, load a different library.
	//		if (System.Runtime.Intrinsics.X86.Avx2.IsSupported) {
	//			return NativeLibrary.Load("nativedep_avx2", assembly, searchPath);
	//		}
	//	}

	//	// Otherwise, fallback to default import resolver.
	//	return IntPtr.Zero;
	//}

	//public static void loadDLLs()
	//{
	//	NativeLibrary.SetDllImportResolver(Assembly.GetExecutingAssembly(), DllImportResolver);

	//	if (Win32.LoadLibrary(@"LoadBackend.dll") == IntPtr.Zero) {
	//		throw new Win32Exception(Marshal.GetLastWin32Error());
	//	}

	//	LoadTheBackend.load();
	//}
}
