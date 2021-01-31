using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using Newtonsoft.Json.Linq;

namespace EmmyLuaLibraryExporter.LuaExporter
{
    public static partial class EmmyLuaExport
    {

        public static string ExportTypeDefinesLuaName = "ExportTypeDefines";
        public static string ExportTypeGlobalVariableLuaName = "ExportTypeGlobalVariable";
        public static string GlobalValueBindToLuaState = "GlobalValueBindToLuaState";

        public static void GenLuaLibrary(string jsonInfoPath)
        {
            var jsonTxt = File.ReadAllText(jsonInfoPath);
            JObject jObject = JObject.Parse(jsonTxt);

            GenLuaClassDefines(jObject["Classes"] as JObject);
            GenLuaEnumDefines(jObject["Enumes"] as JObject);
        }

        private static void GenLuaClassDefines(JObject classesObj)
        {
            if (classesObj == null)
            {
                return;
            }
            // TODO 导出Object
        }

        private static void GenLuaEnumDefines(JObject enumObj)
        {
            if (enumObj == null)
            {
                return;
            }
            // TODO 导出Enum
        }

    }
}