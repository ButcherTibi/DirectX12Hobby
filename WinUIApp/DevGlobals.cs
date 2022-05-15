using System;
using System.IO;


public static class DevGlobals
{
	public static readonly string project_dir;
	public static readonly string output_dir;

	public static readonly string client_side_src;
	public static readonly string client_side_dest;

	static DevGlobals() {
		output_dir = AppDomain.CurrentDomain.BaseDirectory;

		project_dir = System.IO.Path.GetFullPath(System.IO.Path.Combine(
			AppDomain.CurrentDomain.BaseDirectory,
			@"..\..\..\..\..\..\"
		));

		string client_dir_name = "ClientSide";
		client_side_src = Path.Combine(project_dir, client_dir_name);
		client_side_dest = Path.Combine(output_dir, client_dir_name);
	}
}