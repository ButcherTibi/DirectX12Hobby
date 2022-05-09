using System;
using System.Runtime.InteropServices;


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
