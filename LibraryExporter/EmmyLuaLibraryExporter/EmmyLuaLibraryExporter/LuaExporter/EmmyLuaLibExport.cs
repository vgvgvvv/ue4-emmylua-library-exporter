using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
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

            GenLuaClassDefines(jObject["Classes"] as JObject, outputDir);
            GenLuaEnumDefines(jObject["Enumes"] as JObject, outputDir);

            var hintFile = Path.Combine(new DirectoryInfo(outputDir)?.Parent?.FullName, "LuaHint.zip");

            if (File.Exists(hintFile))
            {
                File.Delete(hintFile);
            }
            ZipFile.CreateFromDirectory(outputDir, hintFile);

            Directory.Delete(outputDir, true);
            
            // var luafiles = Directory.GetFiles(outputDir, "*.lua");
            // foreach (var luafile in luafiles)
            // {
            //     File.Delete(luafile);
            // }

        }

        #region Class Generate

        private static void GenLuaClassDefines(JObject classesObj, string outputDir)
        {
            if (classesObj == null)
            {
                return;
            }

            foreach (var jProperty in classesObj.Properties())
            {
                var name = jProperty.Name;
                var value = jProperty.Value.ToObject<JObject>();
                GenSingleLuaClass(name, value, outputDir);
            }
        }

        private static void GenSingleLuaClass(string className, JObject classObj, string outputDir)
        {
            StringBuilder builder = new StringBuilder();
            var superClassName = classObj["Parent"]?.ToObject<string>();

            if (superClassName != null)
            {
                builder.AppendLine($"---@class {className} : {superClassName}");
            }
            else
            {
                builder.AppendLine($"---@class {className}");
            }
            
            var properties = classObj["Properties"].ToObject<JObject>();
            GenSingleLuaClassProperties(properties, builder);

            builder.AppendLine($"{className} = {{}}");

            var functions = classObj["Functions"]?.ToObject<JObject>();
            if (functions != null)
            {
                GenSingleLuaClassFunctions(className, functions, builder);
            }

            if (!Directory.Exists(outputDir))
            {
                Directory.CreateDirectory(outputDir);
            }            
            File.WriteAllText(Path.Combine(outputDir, className + ".lua"), builder.ToString());

        }

        private static void GenSingleLuaClassProperties(JObject properties, StringBuilder builder)
        {
            foreach (var property in properties)
            {
                var propertyName = property.Key;
                var propertyType = property.Value;

                var propertyTypeName = propertyType.ToString();
                
                
                builder.AppendLine($"---@field {propertyName} {propertyTypeName.GetSafeClassName(true)}");
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
                    var paramClassName = param.Value.ToObject<string>();
                    builder.AppendLine($"---@param {param.Name} {paramClassName.GetSafeClassName(true)}");
                }

                if (!string.IsNullOrEmpty(returnType))
                {
                    builder.AppendLine($"---@return {returnType.GetSafeClassName(true)}");
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

        private static void GenLuaEnumDefines(JObject enumsObj, string outputDir)
        {
            if (enumsObj == null)
            {
                return;
            }

            foreach (var jProperty in enumsObj.Properties())
            {
                var name = jProperty.Name;
                var value = jProperty.Value.ToObject<JObject>();
                GenSingleLuaEnumDefine(name, value, outputDir);
            }
        }

        private static void GenSingleLuaEnumDefine(string enumName, JObject enumObj, string outputDir)
        {
            StringBuilder builder = new StringBuilder();
            builder.AppendLine($"---@class {enumName.GetSafeClassName()}");
            var values = enumObj.Properties();
            foreach (var value in values)
            {
                var enumValueName = value.Name;
                if (enumValueName.Contains(":"))
                {
                    enumValueName = enumValueName.Substring(enumValueName.LastIndexOf(':') + 1);
                }
                
                builder.AppendLine($"---@field {enumValueName}");
            }

            builder.AppendLine($"{enumName} = {{}}");
            
            if (!Directory.Exists(outputDir))
            {
                Directory.CreateDirectory(outputDir);
            }      
            File.WriteAllText(Path.Combine(outputDir, enumName + ".lua"), builder.ToString());
        }

        #endregion

        private static string GetSafeClassName(this string className, bool removePrefix = false)
        {
            
            var result = className.Replace("*", "").Replace("&", "");
            
            if (result == "bool")
            {
                return "boolean";
            }
            else if (result == "FString")
            {
                return "string";
            }
            else if (result == "int8" || result == "int16" || result == "int32" || result == "int64" ||
                     result == "uint8" || result == "uint16" || result == "uint32" || result == "uint64" ||
                     result == "float" || result == "double")
            {
                return "number";
            }

            if (removePrefix)
            {
                if (result.StartsWith("I") || result.StartsWith("U") || result.StartsWith("A") ||
                    result.StartsWith("F") || result.StartsWith("T") || result.StartsWith("E"))
                {
                    result = result.Substring(1);
                }
            }
                
            return result;
        }
    
    
    }
}