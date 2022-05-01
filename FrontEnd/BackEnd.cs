using System;
using System.Runtime.InteropServices;


public class BackEnd {

	[DllImport("BackEnd.dll")]
	public static extern
	void initPixDebugger();

	[DllImport("BackEnd.dll")]
	public static extern
	void init();

	[DllImport("BackEnd.dll")]
	public static extern unsafe
	void render(uint width, uint height, byte* r_pixels);
}
