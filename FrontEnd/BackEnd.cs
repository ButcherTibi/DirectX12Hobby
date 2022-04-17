using System.Runtime.InteropServices;


public class BackEnd {

	public static void foo()
	{
		System.Console.WriteLine("");
	}

	[DllImport("BackEnd.dll")]
	public static extern void initPixDebugger();

	[DllImport("BackEnd.dll")]
	public static extern void init();
}
