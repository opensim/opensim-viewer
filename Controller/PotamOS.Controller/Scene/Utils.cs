using System;
using System.Collections.Generic;
using System.Net;
using System.Text;
using System.Reflection;
using System.Xml;
using log4net;
using Xenko.Core.Mathematics;

namespace PotamOS.Controller.Scene
{
    public static class Utils
    {
        private static readonly ILog m_log = LogManager.GetLogger(
                                     MethodBase.GetCurrentMethod().DeclaringType);

        #region From and to bytes
        public static Guid GuidFromBytes(byte[] source, int pos)
        {
            int a = (source[pos + 0] << 24) | (source[pos + 1] << 16) | (source[pos + 2] << 8) | source[pos + 3];
            short b = (short)((source[pos + 4] << 8) | source[pos + 5]);
            short c = (short)((source[pos + 6] << 8) | source[pos + 7]);

            return new Guid(a, b, c, source[pos + 8], source[pos + 9], source[pos + 10], source[pos + 11],
                source[pos + 12], source[pos + 13], source[pos + 14], source[pos + 15]);
        }

        public static Vector3 Vector3FromBytes(byte[] source, int pos)
        {
            float X, Y, Z = 0f;
            if (!BitConverter.IsLittleEndian)
            {
                // Big endian architecture
                byte[] conversionBuffer = new byte[12];

                Buffer.BlockCopy(source, pos, conversionBuffer, 0, 12);

                Array.Reverse(conversionBuffer, 0, 4);
                Array.Reverse(conversionBuffer, 4, 4);
                Array.Reverse(conversionBuffer, 8, 4);

                X = BitConverter.ToSingle(conversionBuffer, 0);
                Y = BitConverter.ToSingle(conversionBuffer, 4);
                Z = BitConverter.ToSingle(conversionBuffer, 8);
            }
            else
            {
                // Little endian architecture
                X = BitConverter.ToSingle(source, pos);
                Y = BitConverter.ToSingle(source, pos + 4);
                Z = BitConverter.ToSingle(source, pos + 8);
            }
            return new Vector3(X, Y, Z);
        }

        public static byte[] Vector3ToBytes(Vector3 v)
        {
            byte[] byteArray = new byte[12];
            Buffer.BlockCopy(BitConverter.GetBytes(v.X), 0, byteArray, 0, 4);
            Buffer.BlockCopy(BitConverter.GetBytes(v.Y), 0, byteArray, 4, 4);
            Buffer.BlockCopy(BitConverter.GetBytes(v.Z), 0, byteArray, 8, 4);

            if (!BitConverter.IsLittleEndian)
            {
                Array.Reverse(byteArray, 0, 4);
                Array.Reverse(byteArray, 4, 4);
                Array.Reverse(byteArray, 8, 4);
            }
            return byteArray;
        }

        public static Color4 Color4FromBytes(byte[] byteArray, int pos, bool inverted)
        {
            float R, G, B, A = 0f;
            const float quanta = 1.0f / 255.0f;

            if (inverted)
            {
                R = (float)(255 - byteArray[pos]) * quanta;
                G = (float)(255 - byteArray[pos + 1]) * quanta;
                B = (float)(255 - byteArray[pos + 2]) * quanta;
                A = (float)(255 - byteArray[pos + 3]) * quanta;
            }
            else
            {
                R = (float)byteArray[pos] * quanta;
                G = (float)byteArray[pos + 1] * quanta;
                B = (float)byteArray[pos + 2] * quanta;
                A = (float)byteArray[pos + 3] * quanta;
            }

            return new Color4(R, G, B, A);
        }

        public static byte[] Color4ToBytes(Color4 c, bool inverted)
        {
            byte[] dest = new byte[4];
            dest[0] = Utils.FloatToByte(c.R, 0f, 1f);
            dest[1] = Utils.FloatToByte(c.G, 0f, 1f);
            dest[2] = Utils.FloatToByte(c.B, 0f, 1f);
            dest[3] = Utils.FloatToByte(c.A, 0f, 1f);

            if (inverted)
            {
                dest[0] = (byte)(255 - dest[0]);
                dest[1] = (byte)(255 - dest[1]);
                dest[2] = (byte)(255 - dest[2]);
                dest[3] = (byte)(255 - dest[3]);
            }
            return dest;
        }
        #endregion from and to bytes

        #region String Arrays

        private static readonly string[] _AssetTypeNames = new string[]
        {
        "texture",    //  0
	    "sound",      //  1
	    "callcard",   //  2
	    "landmark",   //  3
	    "script",     //  4
	    "clothing",   //  5
	    "object",     //  6
	    "notecard",   //  7
	    "category",   //  8
	    String.Empty, //  9
	    "lsltext",    // 10
	    "lslbyte",    // 11
	    "txtr_tga",   // 12
	    "bodypart",   // 13
	    String.Empty, // 14
	    String.Empty, // 15
	    String.Empty, // 16
	    "snd_wav",    // 17
	    "img_tga",    // 18
	    "jpeg",       // 19
	    "animatn",    // 20
	    "gesture",    // 21
	    "simstate",   // 22
        String.Empty, // 23
        "link",       // 24
        "link_f", // 25
        String.Empty, // 26
        String.Empty, // 27
        String.Empty, // 28
        String.Empty, // 29
        String.Empty, // 30
        String.Empty, // 31
        String.Empty, // 32
        String.Empty, // 33
        String.Empty, // 34
        String.Empty, // 35
        String.Empty, // 36
        String.Empty, // 37
        String.Empty, // 38
        String.Empty, // 39
        String.Empty, // 40
        String.Empty, // 41
        String.Empty, // 42
        String.Empty, // 43
        String.Empty, // 44
        String.Empty, // 45
        String.Empty, // 46
        String.Empty, // 47
        String.Empty, // 48
        "mesh",       // 49
        };

        private static readonly string[] _FolderTypeNames = new string[]
        {
        "texture",    //  0
        "sound",      //  1
        "callcard",   //  2
        "landmark",   //  3
        String.Empty, //  4
        "clothing",   //  5
        "object",     //  6
        "notecard",   //  7
        "root_inv",   //  8
        String.Empty, //  9
        "lsltext",    // 10
        String.Empty, // 11
        String.Empty, // 12
        "bodypart",   // 13
        "trash",      // 14
        "snapshot",   // 15
        "lstndfnd",   // 16
        String.Empty, // 17
        String.Empty, // 18
        String.Empty, // 19
        "animatn",    // 20
        "gesture",    // 21
        String.Empty, // 22
        "favorite",   // 23
        String.Empty, // 24
        String.Empty, // 25
        "ensemble",   // 26
        "ensemble",   // 27
        "ensemble",   // 28
        "ensemble",   // 29
        "ensemble",   // 30
        "ensemble",   // 31
        "ensemble",   // 32
        "ensemble",   // 33
        "ensemble",   // 34
        "ensemble",   // 35
        "ensemble",   // 36
        "ensemble",   // 37
        "ensemble",   // 38
        "ensemble",   // 39
        "ensemble",   // 40
        "ensemble",   // 41
        "ensemble",   // 42
        "ensemble",   // 43
        "ensemble",   // 44
        "ensemble",   // 45
        "current",    // 46
        "outfit",     // 47
        "my_otfts",   // 48
        "mesh",       // 49
        "inbox",      // 50
        "outbox",     // 51
        "basic_rt",   // 52
        "merchant",   // 53
        "stock",      // 54
        };

        private static readonly string[] _InventoryTypeNames = new string[]
        {
        "texture",    //  0
	    "sound",      //  1
	    "callcard",   //  2
	    "landmark",   //  3
	    String.Empty, //  4
	    String.Empty, //  5
	    "object",     //  6
	    "notecard",   //  7
	    "category",   //  8
	    "root",       //  9
	    "script",     // 10
	    String.Empty, // 11
	    String.Empty, // 12
	    String.Empty, // 13
	    String.Empty, // 14
	    "snapshot",   // 15
	    String.Empty, // 16
	    "attach",     // 17
	    "wearable",   // 18
	    "animation",  // 19
	    "gesture",    // 20
        String.Empty, // 21
        "mesh"        // 22
        };

        private static readonly string[] _SaleTypeNames = new string[]
        {
        "not",
        "orig",
        "copy",
        "cntn"
        };

        private static readonly string[] _AttachmentPointNames = new string[]
        {
        string.Empty,
        "ATTACH_CHEST",
        "ATTACH_HEAD",
        "ATTACH_LSHOULDER",
        "ATTACH_RSHOULDER",
        "ATTACH_LHAND",
        "ATTACH_RHAND",
        "ATTACH_LFOOT",
        "ATTACH_RFOOT",
        "ATTACH_BACK",
        "ATTACH_PELVIS",
        "ATTACH_MOUTH",
        "ATTACH_CHIN",
        "ATTACH_LEAR",
        "ATTACH_REAR",
        "ATTACH_LEYE",
        "ATTACH_REYE",
        "ATTACH_NOSE",
        "ATTACH_RUARM",
        "ATTACH_RLARM",
        "ATTACH_LUARM",
        "ATTACH_LLARM",
        "ATTACH_RHIP",
        "ATTACH_RULEG",
        "ATTACH_RLLEG",
        "ATTACH_LHIP",
        "ATTACH_LULEG",
        "ATTACH_LLLEG",
        "ATTACH_BELLY",
        "ATTACH_RPEC",
        "ATTACH_LPEC",
        "ATTACH_HUD_CENTER_2",
        "ATTACH_HUD_TOP_RIGHT",
        "ATTACH_HUD_TOP_CENTER",
        "ATTACH_HUD_TOP_LEFT",
        "ATTACH_HUD_CENTER_1",
        "ATTACH_HUD_BOTTOM_LEFT",
        "ATTACH_HUD_BOTTOM",
        "ATTACH_HUD_BOTTOM_RIGHT"
        };

        public static bool InternStrings = false;

        #endregion String Arrays

        #region BytesTo

        /// <summary>
        /// Convert the first two bytes starting in the byte array in
        /// little endian ordering to a signed short integer
        /// </summary>
        /// <param name="bytes">An array two bytes or longer</param>
        /// <returns>A signed short integer, will be zero if a short can't be
        /// read at the given position</returns>
        public static short BytesToInt16(byte[] bytes)
        {
            return BytesToInt16(bytes, 0);
        }

        /// <summary>
        /// Convert the first two bytes starting at the given position in
        /// little endian ordering to a signed short integer
        /// </summary>
        /// <param name="bytes">An array two bytes or longer</param>
        /// <param name="pos">Position in the array to start reading</param>
        /// <returns>A signed short integer, will be zero if a short can't be
        /// read at the given position</returns>
        public static short BytesToInt16(byte[] bytes, int pos)
        {
            if (bytes.Length <= pos + 1) return 0;
            return (short)(bytes[pos] + (bytes[pos + 1] << 8));
        }

        /// <summary>
        /// Convert the first four bytes starting at the given position in
        /// little endian ordering to a signed integer
        /// </summary>
        /// <param name="bytes">An array four bytes or longer</param>
        /// <param name="pos">Position to start reading the int from</param>
        /// <returns>A signed integer, will be zero if an int can't be read
        /// at the given position</returns>
        public static int BytesToInt(byte[] bytes, int pos)
        {
            if (bytes.Length < pos + 4) return 0;
            return (int)(bytes[pos + 0] + (bytes[pos + 1] << 8) + (bytes[pos + 2] << 16) + (bytes[pos + 3] << 24));
        }

        /// <summary>
        /// Convert the first four bytes of the given array in little endian
        /// ordering to a signed integer
        /// </summary>
        /// <param name="bytes">An array four bytes or longer</param>
        /// <returns>A signed integer, will be zero if the array contains
        /// less than four bytes</returns>
        public static int BytesToInt(byte[] bytes)
        {
            return BytesToInt(bytes, 0);
        }

        /// <summary>
        /// Convert the first eight bytes of the given array in little endian
        /// ordering to a signed long integer
        /// </summary>
        /// <param name="bytes">An array eight bytes or longer</param>
        /// <returns>A signed long integer, will be zero if the array contains
        /// less than eight bytes</returns>
        public static long BytesToInt64(byte[] bytes)
        {
            return BytesToInt64(bytes, 0);
        }

        /// <summary>
        /// Convert the first eight bytes starting at the given position in
        /// little endian ordering to a signed long integer
        /// </summary>
        /// <param name="bytes">An array eight bytes or longer</param>
        /// <param name="pos">Position to start reading the long from</param>
        /// <returns>A signed long integer, will be zero if a long can't be read
        /// at the given position</returns>
        public static long BytesToInt64(byte[] bytes, int pos)
        {
            if (bytes.Length < pos + 8) return 0;
            return (long)
                ((long)bytes[pos + 0] +
                ((long)bytes[pos + 1] << 8) +
                ((long)bytes[pos + 2] << 16) +
                ((long)bytes[pos + 3] << 24) +
                ((long)bytes[pos + 4] << 32) +
                ((long)bytes[pos + 5] << 40) +
                ((long)bytes[pos + 6] << 48) +
                ((long)bytes[pos + 7] << 56));
        }

        /// <summary>
        /// Convert the first two bytes starting at the given position in
        /// little endian ordering to an unsigned short
        /// </summary>
        /// <param name="bytes">Byte array containing the ushort</param>
        /// <param name="pos">Position to start reading the ushort from</param>
        /// <returns>An unsigned short, will be zero if a ushort can't be read
        /// at the given position</returns>
        public static ushort BytesToUInt16(byte[] bytes, int pos)
        {
            if (bytes.Length <= pos + 1) return 0;
            return (ushort)(bytes[pos] + (bytes[pos + 1] << 8));
        }

        /// <summary>
        /// Convert two bytes in little endian ordering to an unsigned short
        /// </summary>
        /// <param name="bytes">Byte array containing the ushort</param>
        /// <returns>An unsigned short, will be zero if a ushort can't be
        /// read</returns>
        public static ushort BytesToUInt16(byte[] bytes)
        {
            return BytesToUInt16(bytes, 0);
        }

        /// <summary>
        /// Convert the first four bytes starting at the given position in
        /// little endian ordering to an unsigned integer
        /// </summary>
        /// <param name="bytes">Byte array containing the uint</param>
        /// <param name="pos">Position to start reading the uint from</param>
        /// <returns>An unsigned integer, will be zero if a uint can't be read
        /// at the given position</returns>
        public static uint BytesToUInt(byte[] bytes, int pos)
        {
            if (bytes.Length < pos + 4) return 0;
            return (uint)(bytes[pos + 0] + (bytes[pos + 1] << 8) + (bytes[pos + 2] << 16) + (bytes[pos + 3] << 24));
        }

        /// <summary>
        /// Convert the first four bytes of the given array in little endian
        /// ordering to an unsigned integer
        /// </summary>
        /// <param name="bytes">An array four bytes or longer</param>
        /// <returns>An unsigned integer, will be zero if the array contains
        /// less than four bytes</returns>
        public static uint BytesToUInt(byte[] bytes)
        {
            return BytesToUInt(bytes, 0);
        }

        /// <summary>
        /// Convert the first eight bytes of the given array in little endian
        /// ordering to an unsigned 64-bit integer
        /// </summary>
        /// <param name="bytes">An array eight bytes or longer</param>
        /// <returns>An unsigned 64-bit integer, will be zero if the array
        /// contains less than eight bytes</returns>
        public static ulong BytesToUInt64(byte[] bytes)
        {
            if (bytes.Length < 8) return 0;
            return (ulong)
                ((ulong)bytes[0] +
                ((ulong)bytes[1] << 8) +
                ((ulong)bytes[2] << 16) +
                ((ulong)bytes[3] << 24) +
                ((ulong)bytes[4] << 32) +
                ((ulong)bytes[5] << 40) +
                ((ulong)bytes[6] << 48) +
                ((ulong)bytes[7] << 56));
        }

        /// <summary>
        /// Convert four bytes in little endian ordering to a floating point
        /// value
        /// </summary>
        /// <param name="bytes">Byte array containing a little ending floating
        /// point value</param>
        /// <param name="pos">Starting position of the floating point value in
        /// the byte array</param>
        /// <returns>Single precision value</returns>
        public static float BytesToFloat(byte[] bytes, int pos)
        {
            if (!BitConverter.IsLittleEndian)
            {
                byte[] newBytes = new byte[4];
                Buffer.BlockCopy(bytes, pos, newBytes, 0, 4);
                Array.Reverse(newBytes, 0, 4);
                return BitConverter.ToSingle(newBytes, 0);
            }
            else
            {
                return BitConverter.ToSingle(bytes, pos);
            }
        }

        public static double BytesToDouble(byte[] bytes, int pos)
        {
            if (!BitConverter.IsLittleEndian)
            {
                byte[] newBytes = new byte[8];
                Buffer.BlockCopy(bytes, pos, newBytes, 0, 8);
                Array.Reverse(newBytes, 0, 8);
                return BitConverter.ToDouble(newBytes, 0);
            }
            else
            {
                return BitConverter.ToDouble(bytes, pos);
            }
        }

        #endregion BytesTo

        #region ToBytes

        public static byte[] Int16ToBytes(short value)
        {
            byte[] bytes = new byte[2];
            bytes[0] = (byte)(value % 256);
            bytes[1] = (byte)((value >> 8) % 256);
            return bytes;
        }

        public static void Int16ToBytes(short value, byte[] dest, int pos)
        {
            dest[pos] = (byte)(value % 256);
            dest[pos + 1] = (byte)((value >> 8) % 256);
        }

        public static byte[] UInt16ToBytes(ushort value)
        {
            byte[] bytes = new byte[2];
            bytes[0] = (byte)(value % 256);
            bytes[1] = (byte)((value >> 8) % 256);
            return bytes;
        }

        public static void UInt16ToBytes(ushort value, byte[] dest, int pos)
        {
            dest[pos] = (byte)(value % 256);
            dest[pos + 1] = (byte)((value >> 8) % 256);
        }

        public static void UInt16ToBytesBig(ushort value, byte[] dest, int pos)
        {
            dest[pos] = (byte)((value >> 8) % 256);
            dest[pos + 1] = (byte)(value % 256);
        }

        /// <summary>
        /// Convert an integer to a byte array in little endian format
        /// </summary>
        /// <param name="value">The integer to convert</param>
        /// <returns>A four byte little endian array</returns>
        public static byte[] IntToBytes(int value)
        {
            byte[] bytes = new byte[4];

            bytes[0] = (byte)(value % 256);
            bytes[1] = (byte)((value >> 8) % 256);
            bytes[2] = (byte)((value >> 16) % 256);
            bytes[3] = (byte)((value >> 24) % 256);

            return bytes;
        }

        /// <summary>
        /// Convert an integer to a byte array in big endian format
        /// </summary>
        /// <param name="value">The integer to convert</param>
        /// <returns>A four byte big endian array</returns>
        public static byte[] IntToBytesBig(int value)
        {
            byte[] bytes = new byte[4];

            bytes[0] = (byte)((value >> 24) % 256);
            bytes[1] = (byte)((value >> 16) % 256);
            bytes[2] = (byte)((value >> 8) % 256);
            bytes[3] = (byte)(value % 256);

            return bytes;
        }

        public static void IntToBytes(int value, byte[] dest, int pos)
        {
            dest[pos] = (byte)(value % 256);
            dest[pos + 1] = (byte)((value >> 8) % 256);
            dest[pos + 2] = (byte)((value >> 16) % 256);
            dest[pos + 3] = (byte)((value >> 24) % 256);
        }

        public static byte[] UIntToBytes(uint value)
        {
            byte[] bytes = new byte[4];
            bytes[0] = (byte)(value % 256);
            bytes[1] = (byte)((value >> 8) % 256);
            bytes[2] = (byte)((value >> 16) % 256);
            bytes[3] = (byte)((value >> 24) % 256);
            return bytes;
        }

        public static void UIntToBytes(uint value, byte[] dest, int pos)
        {
            dest[pos] = (byte)(value % 256);
            dest[pos + 1] = (byte)((value >> 8) % 256);
            dest[pos + 2] = (byte)((value >> 16) % 256);
            dest[pos + 3] = (byte)((value >> 24) % 256);
        }

        public static void UIntToBytesBig(uint value, byte[] dest, int pos)
        {
            dest[pos] = (byte)((value >> 24) % 256);
            dest[pos + 1] = (byte)((value >> 16) % 256);
            dest[pos + 2] = (byte)((value >> 8) % 256);
            dest[pos + 3] = (byte)(value % 256);
        }

        /// <summary>
        /// Convert a 64-bit integer to a byte array in little endian format
        /// </summary>
        /// <param name="value">The value to convert</param>
        /// <returns>An 8 byte little endian array</returns>
        public static byte[] Int64ToBytes(long value)
        {
            byte[] bytes = BitConverter.GetBytes(value);
            if (!BitConverter.IsLittleEndian)
                Array.Reverse(bytes);

            return bytes;
        }

        public static void Int64ToBytes(long value, byte[] dest, int pos)
        {
            byte[] bytes = Int64ToBytes(value);
            Buffer.BlockCopy(bytes, 0, dest, pos, 8);
        }

        /// <summary>
        /// Convert a 64-bit unsigned integer to a byte array in little endian
        /// format
        /// </summary>
        /// <param name="value">The value to convert</param>
        /// <returns>An 8 byte little endian array</returns>
        public static byte[] UInt64ToBytes(ulong value)
        {
            byte[] bytes = BitConverter.GetBytes(value);
            if (!BitConverter.IsLittleEndian)
                Array.Reverse(bytes);

            return bytes;
        }

        public static byte[] UInt64ToBytesBig(ulong value)
        {
            byte[] bytes = BitConverter.GetBytes(value);
            if (BitConverter.IsLittleEndian)
                Array.Reverse(bytes);

            return bytes;
        }

        public static void UInt64ToBytes(ulong value, byte[] dest, int pos)
        {
            byte[] bytes = UInt64ToBytes(value);
            Buffer.BlockCopy(bytes, 0, dest, pos, 8);
        }

        public static void UInt64ToBytesBig(ulong value, byte[] dest, int pos)
        {
            byte[] bytes = UInt64ToBytesBig(value);
            Buffer.BlockCopy(bytes, 0, dest, pos, 8);
        }

        /// <summary>
        /// Convert a floating point value to four bytes in little endian
        /// ordering
        /// </summary>
        /// <param name="value">A floating point value</param>
        /// <returns>A four byte array containing the value in little endian
        /// ordering</returns>
        public static byte[] FloatToBytes(float value)
        {
            byte[] bytes = BitConverter.GetBytes(value);
            if (!BitConverter.IsLittleEndian)
                Array.Reverse(bytes);
            return bytes;
        }

        public static void FloatToBytes(float value, byte[] dest, int pos)
        {
            byte[] bytes = FloatToBytes(value);
            Buffer.BlockCopy(bytes, 0, dest, pos, 4);
        }

        public static byte[] DoubleToBytes(double value)
        {
            byte[] bytes = BitConverter.GetBytes(value);
            if (!BitConverter.IsLittleEndian)
                Array.Reverse(bytes);
            return bytes;
        }

        public static byte[] DoubleToBytesBig(double value)
        {
            byte[] bytes = BitConverter.GetBytes(value);
            if (BitConverter.IsLittleEndian)
                Array.Reverse(bytes);
            return bytes;
        }

        public static void DoubleToBytes(double value, byte[] dest, int pos)
        {
            byte[] bytes = DoubleToBytes(value);
            Buffer.BlockCopy(bytes, 0, dest, pos, 8);
        }

        #endregion ToBytes

        #region Strings

        /// <summary>
        /// Converts an unsigned integer to a hexadecimal string
        /// </summary>
        /// <param name="i">An unsigned integer to convert to a string</param>
        /// <returns>A hexadecimal string 10 characters long</returns>
        /// <example>0x7fffffff</example>
        public static string UIntToHexString(uint i)
        {
            return string.Format("{0:x8}", i);
        }

        /// <summary>
        /// Convert a variable length UTF8 byte array to a string
        /// </summary>
        /// <param name="bytes">The UTF8 encoded byte array to convert</param>
        /// <returns>The decoded string</returns>
        public static string BytesToString(byte[] bytes)
        {
            if (bytes.Length > 0 && bytes[bytes.Length - 1] == 0x00)
                return GetString(bytes, 0, bytes.Length - 1);
            else
                return GetString(bytes, 0, bytes.Length);
        }

        public static string BytesToString(byte[] bytes, int index, int count)
        {
            if (bytes.Length > index + count && bytes[index + count - 1] == 0x00)
                return GetString(bytes, index, count - 1);
            else
                return GetString(bytes, index, count);
        }

        private static string GetString(byte[] bytes, int index, int count)
        {
            string cnv = UTF8Encoding.UTF8.GetString(bytes, index, count);
            return InternStrings ? string.Intern(cnv) : cnv;
        }

        /// <summary>
        /// Converts a byte array to a string containing hexadecimal characters
        /// </summary>
        /// <param name="bytes">The byte array to convert to a string</param>
        /// <param name="fieldName">The name of the field to prepend to each
        /// line of the string</param>
        /// <returns>A string containing hexadecimal characters on multiple
        /// lines. Each line is prepended with the field name</returns>
        public static string BytesToHexString(byte[] bytes, string fieldName)
        {
            string cnv = BytesToHexString(bytes, bytes.Length, fieldName);
            return InternStrings ? string.Intern(cnv) : cnv;
        }

        /// <summary>
        /// Converts a byte array to a string containing hexadecimal characters
        /// </summary>
        /// <param name="bytes">The byte array to convert to a string</param>
        /// <param name="length">Number of bytes in the array to parse</param>
        /// <param name="fieldName">A string to prepend to each line of the hex
        /// dump</param>
        /// <returns>A string containing hexadecimal characters on multiple
        /// lines. Each line is prepended with the field name</returns>
        public static string BytesToHexString(byte[] bytes, int length, string fieldName)
        {
            StringBuilder output = new StringBuilder();

            for (int i = 0; i < length; i += 16)
            {
                if (i != 0)
                    output.Append('\n');

                if (!String.IsNullOrEmpty(fieldName))
                {
                    output.Append(fieldName);
                    output.Append(": ");
                }

                for (int j = 0; j < 16; j++)
                {
                    if ((i + j) < length)
                    {
                        if (j != 0)
                            output.Append(' ');

                        output.Append(String.Format("{0:X2}", bytes[i + j]));
                    }
                }
            }

            return output.ToString();
        }

        public static readonly byte[] EmptyBytes = new byte[0];

        /// <summary>
        /// Convert a string to a UTF8 encoded byte array
        /// </summary>
        /// <param name="str">The string to convert</param>
        /// <returns>A null-terminated UTF8 byte array</returns>
        public static byte[] StringToBytes(string str)
        {
            if (String.IsNullOrEmpty(str)) { return EmptyBytes; }
            if (!str.EndsWith("\0")) { str += "\0"; }
            return System.Text.UTF8Encoding.UTF8.GetBytes(str);
        }

        /// <summary>
        /// Converts a string containing hexadecimal characters to a byte array
        /// </summary>
        /// <param name="hexString">String containing hexadecimal characters</param>
        /// <param name="handleDirty">If true, gracefully handles null, empty and
        /// uneven strings as well as stripping unconvertable characters</param>
        /// <returns>The converted byte array</returns>
        public static byte[] HexStringToBytes(string hexString, bool handleDirty)
        {
            if (handleDirty)
            {
                if (String.IsNullOrEmpty(hexString))
                    return Utils.EmptyBytes;

                StringBuilder stripped = new StringBuilder(hexString.Length);
                char c;

                // remove all non A-F, 0-9, characters
                for (int i = 0; i < hexString.Length; i++)
                {
                    c = hexString[i];
                    if (IsHexDigit(c))
                        stripped.Append(c);
                }

                hexString = stripped.ToString();

                // if odd number of characters, discard last character
                if (hexString.Length % 2 != 0)
                {
                    hexString = hexString.Substring(0, hexString.Length - 1);
                }
            }

            int byteLength = hexString.Length / 2;
            byte[] bytes = new byte[byteLength];
            int j = 0;

            for (int i = 0; i < bytes.Length; i++)
            {
                bytes[i] = HexToByte(hexString.Substring(j, 2));
                j += 2;
            }

            return bytes;
        }

        /// <summary>
        /// Returns true is c is a hexadecimal digit (A-F, a-f, 0-9)
        /// </summary>
        /// <param name="c">Character to test</param>
        /// <returns>true if hex digit, false if not</returns>
        private static bool IsHexDigit(Char c)
        {
            const int numA = 65;
            const int num0 = 48;

            int numChar;

            c = Char.ToUpper(c);
            numChar = Convert.ToInt32(c);

            if (numChar >= numA && numChar < (numA + 6))
                return true;
            else if (numChar >= num0 && numChar < (num0 + 10))
                return true;
            else
                return false;
        }

        /// <summary>
        /// Converts 1 or 2 character string into equivalant byte value
        /// </summary>
        /// <param name="hex">1 or 2 character string</param>
        /// <returns>byte</returns>
        private static byte HexToByte(string hex)
        {
            if (hex.Length > 2 || hex.Length <= 0)
                throw new ArgumentException("hex must be 1 or 2 characters in length");
            byte newByte = Byte.Parse(hex, System.Globalization.NumberStyles.HexNumber);
            return newByte;
        }

        #endregion Strings

        #region Packed Values

        /// <summary>
        /// Convert a float value to a byte given a minimum and maximum range
        /// </summary>
        /// <param name="val">Value to convert to a byte</param>
        /// <param name="lower">Minimum value range</param>
        /// <param name="upper">Maximum value range</param>
        /// <returns>A single byte representing the original float value</returns>
        public static byte FloatToByte(float val, float lower, float upper)
        {
            val = Utils.Clamp(val, lower, upper);
            // Normalize the value
            val -= lower;
            val /= (upper - lower);

            return (byte)Math.Floor(val * (float)byte.MaxValue);
        }

        /// <summary>
        /// Convert a byte to a float value given a minimum and maximum range
        /// </summary>
        /// <param name="bytes">Byte array to get the byte from</param>
        /// <param name="pos">Position in the byte array the desired byte is at</param>
        /// <param name="lower">Minimum value range</param>
        /// <param name="upper">Maximum value range</param>
        /// <returns>A float value inclusively between lower and upper</returns>
        public static float ByteToFloat(byte[] bytes, int pos, float lower, float upper)
        {
            if (bytes.Length <= pos) return 0;
            return ByteToFloat(bytes[pos], lower, upper);
        }

        /// <summary>
        /// Convert a byte to a float value given a minimum and maximum range
        /// </summary>
        /// <param name="val">Byte to convert to a float value</param>
        /// <param name="lower">Minimum value range</param>
        /// <param name="upper">Maximum value range</param>
        /// <returns>A float value inclusively between lower and upper</returns>
        public static float ByteToFloat(byte val, float lower, float upper)
        {
            const float ONE_OVER_BYTEMAX = 1.0f / (float)byte.MaxValue;

            float fval = (float)val * ONE_OVER_BYTEMAX;
            float delta = (upper - lower);
            fval *= delta;
            fval += lower;

            // Test for values very close to zero
            float error = delta * ONE_OVER_BYTEMAX;
            if (Math.Abs(fval) < error)
                fval = 0.0f;

            return fval;
        }

        public static float UInt16ToFloat(byte[] bytes, int pos, float lower, float upper)
        {
            ushort val = BytesToUInt16(bytes, pos);
            return UInt16ToFloat(val, lower, upper);
        }

        public static float UInt16ToFloat(ushort val, float lower, float upper)
        {
            const float ONE_OVER_U16_MAX = 1.0f / (float)UInt16.MaxValue;

            float fval = (float)val * ONE_OVER_U16_MAX;
            float delta = upper - lower;
            fval *= delta;
            fval += lower;

            // Make sure zeroes come through as zero
            float maxError = delta * ONE_OVER_U16_MAX;
            if (Math.Abs(fval) < maxError)
                fval = 0.0f;

            return fval;
        }

        public static ushort FloatToUInt16(float value, float lower, float upper)
        {
            float delta = upper - lower;
            value -= lower;
            value /= delta;
            value *= (float)UInt16.MaxValue;

            return (ushort)value;
        }

        #endregion Packed Values

        #region TryParse

        /// <summary>
        /// Attempts to parse a floating point value from a string, using an
        /// EN-US number format
        /// </summary>
        /// <param name="s">String to parse</param>
        /// <param name="result">Resulting floating point number</param>
        /// <returns>True if the parse was successful, otherwise false</returns>
        public static bool TryParseSingle(string s, out float result)
        {
            return Single.TryParse(s, System.Globalization.NumberStyles.Float, EnUsCulture.NumberFormat, out result);
        }

        /// <summary>
        /// Attempts to parse a floating point value from a string, using an
        /// EN-US number format
        /// </summary>
        /// <param name="s">String to parse</param>
        /// <param name="result">Resulting floating point number</param>
        /// <returns>True if the parse was successful, otherwise false</returns>
        public static bool TryParseDouble(string s, out double result)
        {
            // NOTE: Double.TryParse can't parse Double.[Min/Max]Value.ToString(), see:
            // http://blogs.msdn.com/bclteam/archive/2006/05/24/598169.aspx
            return Double.TryParse(s, System.Globalization.NumberStyles.Float, EnUsCulture.NumberFormat, out result);
        }

        /// <summary>
        /// Tries to parse an unsigned 32-bit integer from a hexadecimal string
        /// </summary>
        /// <param name="s">String to parse</param>
        /// <param name="result">Resulting integer</param>
        /// <returns>True if the parse was successful, otherwise false</returns>
        public static bool TryParseHex(string s, out uint result)
        {
            return UInt32.TryParse(s, System.Globalization.NumberStyles.HexNumber, EnUsCulture.NumberFormat, out result);
        }

        #endregion TryParse

        #region Miscellaneous

        /// <summary>
        /// Copy a byte array
        /// </summary>
        /// <param name="bytes">Byte array to copy</param>
        /// <returns>A copy of the given byte array</returns>
        public static byte[] CopyBytes(byte[] bytes)
        {
            if (bytes == null)
                return null;

            byte[] newBytes = new byte[bytes.Length];
            Buffer.BlockCopy(bytes, 0, newBytes, 0, bytes.Length);
            return newBytes;
        }

        /// <summary>
        /// Packs to 32-bit unsigned integers in to a 64-bit unsigned integer
        /// </summary>
        /// <param name="a">The left-hand (or X) value</param>
        /// <param name="b">The right-hand (or Y) value</param>
        /// <returns>A 64-bit integer containing the two 32-bit input values</returns>
        public static ulong UIntsToLong(uint a, uint b)
        {
            return ((ulong)a << 32) | (ulong)b;
        }

        /// <summary>
        /// Unpacks two 32-bit unsigned integers from a 64-bit unsigned integer
        /// </summary>
        /// <param name="a">The 64-bit input integer</param>
        /// <param name="b">The left-hand (or X) output value</param>
        /// <param name="c">The right-hand (or Y) output value</param>
        public static void LongToUInts(ulong a, out uint b, out uint c)
        {
            b = (uint)(a >> 32);
            c = (uint)(a & 0x00000000FFFFFFFF);
        }

        /// <summary>
        /// Convert an IP address object to an unsigned 32-bit integer
        /// </summary>
        /// <param name="address">IP address to convert</param>
        /// <returns>32-bit unsigned integer holding the IP address bits</returns>
        public static uint IPToUInt(System.Net.IPAddress address)
        {
            byte[] bytes = address.GetAddressBytes();
            return (uint)((bytes[3] << 24) + (bytes[2] << 16) + (bytes[1] << 8) + bytes[0]);
        }

        /// <summary>UNIX epoch in DateTime format</summary>
        public static readonly DateTime Epoch = new DateTime(1970, 1, 1);

        /// <summary>
        /// Gets a unix timestamp for the current time
        /// </summary>
        /// <returns>An unsigned integer representing a unix timestamp for now</returns>
        public static uint GetUnixTime()
        {
            return (uint)(DateTime.UtcNow - Epoch).TotalSeconds;
        }

        /// <summary>
        /// Convert a UNIX timestamp to a native DateTime object
        /// </summary>
        /// <param name="timestamp">An unsigned integer representing a UNIX
        /// timestamp</param>
        /// <returns>A DateTime object containing the same time specified in
        /// the given timestamp</returns>
        public static DateTime UnixTimeToDateTime(uint timestamp)
        {
            DateTime dateTime = Epoch;

            // Add the number of seconds in our UNIX timestamp
            dateTime = dateTime.AddSeconds(timestamp);

            return dateTime;
        }

        /// <summary>
        /// Convert a UNIX timestamp to a native DateTime object
        /// </summary>
        /// <param name="timestamp">A signed integer representing a UNIX
        /// timestamp</param>
        /// <returns>A DateTime object containing the same time specified in
        /// the given timestamp</returns>
        public static DateTime UnixTimeToDateTime(int timestamp)
        {
            return UnixTimeToDateTime((uint)timestamp);
        }

        /// <summary>
        /// Convert a native DateTime object to a UNIX timestamp
        /// </summary>
        /// <param name="time">A DateTime object you want to convert to a 
        /// timestamp</param>
        /// <returns>An unsigned integer representing a UNIX timestamp</returns>
        public static uint DateTimeToUnixTime(DateTime time)
        {
            TimeSpan ts = (time - new DateTime(1970, 1, 1, 0, 0, 0));
            return (uint)ts.TotalSeconds;
        }

        /// <summary>
        /// Swap two values
        /// </summary>
        /// <typeparam name="T">Type of the values to swap</typeparam>
        /// <param name="lhs">First value</param>
        /// <param name="rhs">Second value</param>
        public static void Swap<T>(ref T lhs, ref T rhs)
        {
            T temp = lhs;
            lhs = rhs;
            rhs = temp;
        }

        /// <summary>
        /// Try to parse an enumeration value from a string
        /// </summary>
        /// <typeparam name="T">Enumeration type</typeparam>
        /// <param name="strType">String value to parse</param>
        /// <param name="result">Enumeration value on success</param>
        /// <returns>True if the parsing succeeded, otherwise false</returns>
        public static bool EnumTryParse<T>(string strType, out T result)
        {
            Type t = typeof(T);

            if (Enum.IsDefined(t, strType))
            {
                result = (T)Enum.Parse(t, strType, true);
                return true;
            }
            else
            {
                foreach (string value in Enum.GetNames(typeof(T)))
                {
                    if (value.Equals(strType, StringComparison.OrdinalIgnoreCase))
                    {
                        result = (T)Enum.Parse(typeof(T), value);
                        return true;
                    }
                }
                result = default(T);
                return false;
            }
        }

        /// <summary>
        /// Swaps the high and low words in a byte. Converts aaaabbbb to bbbbaaaa
        /// </summary>
        /// <param name="value">Byte to swap the words in</param>
        /// <returns>Byte value with the words swapped</returns>
        public static byte SwapWords(byte value)
        {
            return (byte)(((value & 0xF0) >> 4) | ((value & 0x0F) << 4));
        }

        /// <summary>
        /// Attempts to convert a string representation of a hostname or IP
        /// address to a <seealso cref="System.Net.IPAddress"/>
        /// </summary>
        /// <param name="hostname">Hostname to convert to an IPAddress</param>
        /// <returns>Converted IP address object, or null if the conversion
        /// failed</returns>
        public static IPAddress HostnameToIPv4(string hostname)
        {
            // Is it already a valid IP?
            IPAddress ip;
            if (IPAddress.TryParse(hostname, out ip))
                return ip;

            IPAddress[] hosts = Dns.GetHostEntry(hostname).AddressList;

            for (int i = 0; i < hosts.Length; i++)
            {
                IPAddress host = hosts[i];

                if (host.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
                    return host;
            }

            return null;
        }

        #endregion Miscellaneous

        #region Math

        /// <summary>
        /// Clamp a given value between a range
        /// </summary>
        /// <param name="value">Value to clamp</param>
        /// <param name="min">Minimum allowable value</param>
        /// <param name="max">Maximum allowable value</param>
        /// <returns>A value inclusively between lower and upper</returns>
        public static float Clamp(float value, float min, float max)
        {
            // First we check to see if we're greater than the max
            value = (value > max) ? max : value;

            // Then we check to see if we're less than the min.
            value = (value < min) ? min : value;

            // There's no check to see if min > max.
            return value;
        }

        /// <summary>
        /// Clamp a given value between a range
        /// </summary>
        /// <param name="value">Value to clamp</param>
        /// <param name="min">Minimum allowable value</param>
        /// <param name="max">Maximum allowable value</param>
        /// <returns>A value inclusively between lower and upper</returns>
        public static double Clamp(double value, double min, double max)
        {
            // First we check to see if we're greater than the max
            value = (value > max) ? max : value;

            // Then we check to see if we're less than the min.
            value = (value < min) ? min : value;

            // There's no check to see if min > max.
            return value;
        }

        /// <summary>
        /// Clamp a given value between a range
        /// </summary>
        /// <param name="value">Value to clamp</param>
        /// <param name="min">Minimum allowable value</param>
        /// <param name="max">Maximum allowable value</param>
        /// <returns>A value inclusively between lower and upper</returns>
        public static int Clamp(int value, int min, int max)
        {
            // First we check to see if we're greater than the max
            value = (value > max) ? max : value;

            // Then we check to see if we're less than the min.
            value = (value < min) ? min : value;

            // There's no check to see if min > max.
            return value;
        }

        /// <summary>
        /// Round a floating-point value to the nearest integer
        /// </summary>
        /// <param name="val">Floating point number to round</param>
        /// <returns>Integer</returns>
        public static int Round(float val)
        {
            return (int)Math.Floor(val + 0.5f);
        }

        /// <summary>
        /// Test if a single precision float is a finite number
        /// </summary>
        public static bool IsFinite(float value)
        {
            return !(Single.IsNaN(value) || Single.IsInfinity(value));
        }

        /// <summary>
        /// Test if a double precision float is a finite number
        /// </summary>
        public static bool IsFinite(double value)
        {
            return !(Double.IsNaN(value) || Double.IsInfinity(value));
        }

        /// <summary>
        /// Get the distance between two floating-point values
        /// </summary>
        /// <param name="value1">First value</param>
        /// <param name="value2">Second value</param>
        /// <returns>The distance between the two values</returns>
        public static float Distance(float value1, float value2)
        {
            return Math.Abs(value1 - value2);
        }

        public static float Hermite(float value1, float tangent1, float value2, float tangent2, float amount)
        {
            // All transformed to double not to lose precission
            // Otherwise, for high numbers of param:amount the result is NaN instead of Infinity
            double v1 = value1, v2 = value2, t1 = tangent1, t2 = tangent2, s = amount, result;
            double sCubed = s * s * s;
            double sSquared = s * s;

            if (amount == 0f)
                result = value1;
            else if (amount == 1f)
                result = value2;
            else
                result = (2d * v1 - 2d * v2 + t2 + t1) * sCubed +
                    (3d * v2 - 3d * v1 - 2d * t1 - t2) * sSquared +
                    t1 * s + v1;
            return (float)result;
        }

        public static double Hermite(double value1, double tangent1, double value2, double tangent2, double amount)
        {
            // All transformed to double not to lose precission
            // Otherwise, for high numbers of param:amount the result is NaN instead of Infinity
            double v1 = value1, v2 = value2, t1 = tangent1, t2 = tangent2, s = amount, result;
            double sCubed = s * s * s;
            double sSquared = s * s;

            if (amount == 0d)
                result = value1;
            else if (amount == 1f)
                result = value2;
            else
                result = (2d * v1 - 2d * v2 + t2 + t1) * sCubed +
                    (3d * v2 - 3d * v1 - 2d * t1 - t2) * sSquared +
                    t1 * s + v1;
            return result;
        }

        public static float Lerp(float value1, float value2, float amount)
        {
            return value1 + (value2 - value1) * amount;
        }

        public static double Lerp(double value1, double value2, double amount)
        {
            return value1 + (value2 - value1) * amount;
        }

        public static float SmoothStep(float value1, float value2, float amount)
        {
            // It is expected that 0 < amount < 1
            // If amount < 0, return value1
            // If amount > 1, return value2
            float result = Utils.Clamp(amount, 0f, 1f);
            return Utils.Hermite(value1, 0f, value2, 0f, result);
        }

        public static double SmoothStep(double value1, double value2, double amount)
        {
            // It is expected that 0 < amount < 1
            // If amount < 0, return value1
            // If amount > 1, return value2
            double result = Utils.Clamp(amount, 0f, 1f);
            return Utils.Hermite(value1, 0f, value2, 0f, result);
        }

        public static float ToDegrees(float radians)
        {
            // This method uses double precission internally,
            // though it returns single float
            // Factor = 180 / pi
            return (float)(radians * 57.295779513082320876798154814105);
        }

        public static float ToRadians(float degrees)
        {
            // This method uses double precission internally,
            // though it returns single float
            // Factor = pi / 180
            return (float)(degrees * 0.017453292519943295769236907684886);
        }

        /// <summary>Provide a single instance of the MD5 class to avoid making
        /// duplicate copies and handle thread safety</summary>
        private static readonly System.Security.Cryptography.MD5 MD5Builder =
            new System.Security.Cryptography.MD5CryptoServiceProvider();

        /// <summary>Provide a single instance of the SHA-1 class to avoid
        /// making duplicate copies and handle thread safety</summary>
        private static readonly System.Security.Cryptography.SHA1 SHA1Builder =
            new System.Security.Cryptography.SHA1CryptoServiceProvider();

        private static readonly System.Security.Cryptography.SHA256 SHA256Builder =
            new System.Security.Cryptography.SHA256Managed();

        /// <summary>
        /// Compute the MD5 hash for a byte array
        /// </summary>
        /// <param name="data">Byte array to compute the hash for</param>
        /// <returns>MD5 hash of the input data</returns>
        public static byte[] MD5(byte[] data)
        {
            lock (MD5Builder)
                return MD5Builder.ComputeHash(data);
        }

        /// <summary>
        /// Compute the SHA1 hash for a byte array
        /// </summary>
        /// <param name="data">Byte array to compute the hash for</param>
        /// <returns>SHA1 hash of the input data</returns>
        public static byte[] SHA1(byte[] data)
        {
            lock (SHA1Builder)
                return SHA1Builder.ComputeHash(data);
        }

        /// <summary>
        /// Calculate the SHA1 hash of a given string
        /// </summary>
        /// <param name="value">The string to hash</param>
        /// <returns>The SHA1 hash as a string</returns>
        public static string SHA1String(string value)
        {
            StringBuilder digest = new StringBuilder(40);
            byte[] hash = SHA1(Encoding.UTF8.GetBytes(value));

            // Convert the hash to a hex string
            foreach (byte b in hash)
                digest.AppendFormat(Utils.EnUsCulture, "{0:x2}", b);

            return digest.ToString();
        }

        /// <summary>
        /// Compute the SHA256 hash for a byte array
        /// </summary>
        /// <param name="data">Byte array to compute the hash for</param>
        /// <returns>SHA256 hash of the input data</returns>
        public static byte[] SHA256(byte[] data)
        {
            lock (SHA256Builder)
                return SHA256Builder.ComputeHash(data);
        }

        /// <summary>
        /// Calculate the SHA256 hash of a given string
        /// </summary>
        /// <param name="value">The string to hash</param>
        /// <returns>The SHA256 hash as a string</returns>
        public static string SHA256String(string value)
        {
            StringBuilder digest = new StringBuilder(64);
            byte[] hash = SHA256(Encoding.UTF8.GetBytes(value));

            // Convert the hash to a hex string
            foreach (byte b in hash)
                digest.AppendFormat(Utils.EnUsCulture, "{0:x2}", b);

            return digest.ToString();
        }

        /// <summary>
        /// Calculate the MD5 hash of a given string
        /// </summary>
        /// <param name="password">The password to hash</param>
        /// <returns>An MD5 hash in string format, with $1$ prepended</returns>
        public static string MD5(string password)
        {
            StringBuilder digest = new StringBuilder(32);
            byte[] hash = MD5(ASCIIEncoding.Default.GetBytes(password));

            // Convert the hash to a hex string
            foreach (byte b in hash)
                digest.AppendFormat(Utils.EnUsCulture, "{0:x2}", b);

            return "$1$" + digest.ToString();
        }

        /// <summary>Provide a single instance of the CultureInfo class to
        /// help parsing in situations where the grid assumes an en-us 
        /// culture</summary>
        public static readonly System.Globalization.CultureInfo EnUsCulture =
            new System.Globalization.CultureInfo("en-us", false);

        /// <summary>
        /// Calculate the MD5 hash of a given string
        /// </summary>
        /// <param name="value">The string to hash</param>
        /// <returns>The MD5 hash as a string</returns>
        public static string MD5String(string value)
        {
            StringBuilder digest = new StringBuilder(32);
            byte[] hash = MD5(Encoding.UTF8.GetBytes(value));

            // Convert the hash to a hex string
            foreach (byte b in hash)
                digest.AppendFormat(EnUsCulture, "{0:x2}", b);

            return digest.ToString();
        }

        /// <summary>Provide a single instance of a random number generator
        /// to avoid making duplicate copies and handle thread safety</summary>
        private static readonly Random RNG = new Random();

        /// <summary>
        /// Generate a random double precision floating point value
        /// </summary>
        /// <returns>Random value of type double</returns>
        public static double RandomDouble()
        {
            lock (RNG)
                return RNG.NextDouble();
        }

        #endregion Math


    }
}
