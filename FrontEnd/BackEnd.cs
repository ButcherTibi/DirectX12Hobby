using System;
using System.Runtime.InteropServices;


public class Win32
{
	[DllImport("user32.dll", EntryPoint = "CreateWindowEx", CharSet = CharSet.Unicode)]
	public static extern IntPtr CreateWindowEx(
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
	public static extern bool DestroyWindow(IntPtr hwnd);

	[DllImport("User32.dll")]
	public static extern bool SetCursorPos(int X, int Y);

	[StructLayout(LayoutKind.Sequential)]
	public struct Rect
	{
		public Int32 left;
		public Int32 top;
		public Int32 right;
		public Int32 bottom;
	}

	[DllImport("User32.dll")]
	public unsafe static extern bool ClipCursor(Rect* rect);
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
