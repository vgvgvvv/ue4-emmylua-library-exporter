using System;
using EmmyLuaLibraryExporter.LuaExporter;

namespace EmmyLuaLibraryExporter
{
    class Program
    {
        static void Main(string[] args)
        {
            string libraryjson =
                @"D:\Documents\MyProjects\TestProjects\TestResetCoreUnreal\RemoteControl\Plugins\EmmyLuaLibraryExporter\Resources\library.json";
            string outputDir =
                @"D:\Documents\MyProjects\TestProjects\TestResetCoreUnreal\RemoteControl\Plugins\EmmyLuaLibraryExporter\Resources";
            EmmyLuaExport.GenLuaDefine(libraryjson, outputDir);
            Console.WriteLine("Export Finish!!");
        }
    }
}