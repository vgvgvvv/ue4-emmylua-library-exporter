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
    public static class EmmyLuaExport
    {

        public static string ExportTypeDefinesLuaName = "ExportTypeDefines";
        public static string ExportTypeGlobalVariableLuaName = "ExportTypeGlobalVariable";

        public static void GenLuaDefine(string jsonInfoPath, string outputDir)
        {
            var jsonTxt = File.ReadAllText(jsonInfoPath);
            JObject jObject = JObject.Parse(jsonTxt);

            StringBuilder defineTypeBuilder = new StringBuilder();
            GenLuaClassDefines(jObject["Classes"] as JObject, defineTypeBuilder);
            GenLuaEnumDefines(jObject["Enumes"] as JObject, defineTypeBuilder);
            
            File.WriteAllText(Path.Combine(outputDir, ExportTypeDefinesLuaName + ".lua"), defineTypeBuilder.ToString());
            
        }

        #region Class Generate

        private static void GenLuaClassDefines(JObject classesObj, StringBuilder builder)
        {
            if (classesObj == null)
            {
                return;
            }

            foreach (var jProperty in classesObj.Properties())
            {
                var name = jProperty.Name;
                var value = jProperty.Value.ToObject<JObject>();
                GenSingleLuaClass(name, value, builder);
                builder.AppendLine("\n--------------------------------------------------------\n");
            }
        }

        private static void GenSingleLuaClass(string className, JObject classObj, StringBuilder builder)
        {
            builder.AppendLine($"---@class {className}");
            
            var properties = classObj["Properties"].ToObject<JObject>();
            GenSingleLuaClassProperties(properties, builder);

            builder.AppendLine($"{className} = {{}}");

            var functions = classObj["Functions"]?.ToObject<JObject>();
            if (functions != null)
            {
                GenSingleLuaClassFunctions(className, functions, builder);
            }
        }

        private static void GenSingleLuaClassProperties(JObject properties, StringBuilder builder)
        {
            foreach (var property in properties)
            {
                var propertyName = property.Key;
                var propertyType = property.Value;
                builder.AppendLine($"---@field {propertyName} {propertyType}");
            }
        }

        private static void GenSingleLuaClassFunctions(string className, JObject functions, StringBuilder builder)
        {
            foreach (var function in functions)
            {
                var functionName = function.Key;
                var overrides = function.Value["Overrides"].ToArray();
                var isStatic = function.Value["IsStatic"].ToObject<bool>();
                if (overrides.Length == 0)
                {
                    continue;
                }

                var firstFunction = overrides[0] as JObject;
                var returnType = firstFunction["ReturnType"].ToObject<string>();
                var paramInfos = firstFunction["Params"].ToObject<JObject>();
               
                foreach (var param in paramInfos.Properties())
                {
                    builder.AppendLine($"---@param {param.Name} {param.Value.ToObject<string>()}");
                }

                if (!string.IsNullOrEmpty(returnType))
                {
                    builder.AppendLine($"---@return {returnType}");
                }

                for (int i = 1; i < overrides.Length - 1; i++)
                {
                    StringBuilder overloadArgsBuilder = new StringBuilder();
                    {
                        bool isFirst = true;
                        foreach (var paramInfo in paramInfos)
                        {
                            if (!isFirst)
                            {
                                overloadArgsBuilder.Append(", ");
                            }
                            overloadArgsBuilder.Append(paramInfo.Key).Append(" : ").Append(paramInfo.Value);
                            isFirst = false;
                        }
                    }
                    var overload = overrides[i] as JObject;

                    builder.AppendLine($"---@overload func({overloadArgsBuilder})");
                }
                
                var dot = isStatic ? "." : ":";

                var argsBuilder = new StringBuilder();
                {
                    bool isFirst = true;
                    foreach (var paramInfo in paramInfos)
                    {
                        if (!isFirst)
                        {
                            argsBuilder.Append(", ");
                        }
                        argsBuilder.Append(paramInfo.Key);
                        isFirst = false;
                    }
                }
                builder.AppendLine($"function {className}{dot}{functionName}({argsBuilder}) end");

            }
        }

        #endregion
        
        

        #region Enum Generate

        private static void GenLuaEnumDefines(JObject enumsObj, StringBuilder builder)
        {
            if (enumsObj == null)
            {
                return;
            }

            foreach (var jProperty in enumsObj.Properties())
            {
                var name = jProperty.Name;
                var value = jProperty.Value.ToObject<JObject>();
                GenSingleLuaEnumDefine(name, value, builder);
                builder.AppendLine("\n--------------------------------------------------------\n");
            }
        }

        private static void GenSingleLuaEnumDefine(string enumName, JObject enumObj, StringBuilder builder)
        {
            builder.AppendLine($"---@class {enumName}");
            var values = enumObj.Properties();
            foreach (var value in values)
            {
                builder.AppendLine($"---@field {value.Name}");
            }

            builder.AppendLine($"{enumName} = {{}}");
        }

        #endregion
       

    }
}