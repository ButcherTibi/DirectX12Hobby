using System;
using System.Runtime.InteropServices;


public class Win32
{
	[DllImport("user32.dll", EntryPoint = "CreateWindowEx", CharSet = CharSet.Unicode)]
	internal static extern IntPtr CreateWindowEx(
		int dwExStyle,
		string lpszClassName,
		string lpszWindowName,
		int style,
		int x, int y,
		int width, int height,
		IntPtr hwndParent,
		IntPtr hMenu,
		IntPtr hInstance,
		IntPtr lpParam
	);

	[DllImport("user32.dll", EntryPoint = "DestroyWindow", CharSet = CharSet.Unicode)]
	internal static extern bool DestroyWindow(IntPtr hwnd);
}

public class BackEnd {

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
}
