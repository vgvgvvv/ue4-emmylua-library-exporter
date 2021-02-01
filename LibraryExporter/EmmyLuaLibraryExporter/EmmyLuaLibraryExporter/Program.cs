using System;
using System.Collections.Generic;
using System.IO;
using EmmyLuaLibraryExporter.LuaExporter;

namespace EmmyLuaLibraryExporter
{
    class Program
    {
        private static readonly Dictionary<string, string> ArgMap = new Dictionary<string, string>();
        
        static void Main(string[] args)
        {
            foreach (var arg in args)
            {
                var result = arg.Split("=");
                if (result.Length > 1)
                {
                    ArgMap.Add(result[0], result[1]);
                }
                else
                {
                    ArgMap.Add(arg, "");
                }
            }

            if(!ArgMap.TryGetValue("in", out var libraryjson))
            {
                libraryjson = Path.Combine(Environment.CurrentDirectory, "library.json");
            }
            if(!ArgMap.TryGetValue("out", out var outputDir))
            {
                outputDir = Environment.CurrentDirectory;
            }
            EmmyLuaExport.GenLuaDefine(libraryjson, outputDir);
            Console.WriteLine("Export Finish!!");
        }
    }
}