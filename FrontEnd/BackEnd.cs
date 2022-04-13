using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;


public class BackEnd {
	
	[DllImport("BackEnd.dll")]
	public static extern void initPixDebugger();

	[DllImport("BackEnd.dll")]
	public static extern void initRender();

	[DllImport("user32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
	public static extern int MessageBox(IntPtr hWnd, string lpText, string lpCaption, uint uType);
}
