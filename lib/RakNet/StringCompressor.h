/// \file
/// \brief \b Compresses/Decompresses ASCII strings and writes/reads them to BitStream class instances.  You can use this to easily serialize and deserialize your own strings.
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.rakkarsoft.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __STRING_COMPRESSOR_H
#define __STRING_COMPRESSOR_H

/// Forward declaration
class BitStream;

class HuffmanEncodingTree;

/// \brief Writes and reads strings to and from bitstreams.
///
/// Only works with ASCII strings.  The default compression is for English.
/// You can call GenerateTreeFromStrings to compress and decompress other languages efficiently as well.
class StringCompressor
{
public:
	
	/// Destructor	
	~StringCompressor();
	
	/// static function because only static functions can access static members
	/// The RakPeer constructor adds a reference to this class, so don't call this until an instance of RakPeer exists, or unless you call AddReference yourself.
	/// \return the unique instance of the StringCompressor 
	static StringCompressor* Instance(void);

 	/// Writes input to output, compressed.  Takes care of the null terminator for you.
	/// \param[in] input Pointer to an ASCII string
	/// \param[in] maxCharsToWrite The max number of bytes to write of \a input.  Use 0 to mean no limit.
	/// \param[out] output The bitstream to write the compressed string to
	/// \param[in] languageID Which language to use
	void EncodeString( const char *input, int maxCharsToWrite, BitStream *output, int languageID=0 );
	
	/// Writes input to output, uncompressed.  Takes care of the null terminator for you.
	/// \param[out] output A block of bytes to receive the output
	/// \param[in] maxCharsToWrite Size, in bytes, of \a output .  A NULL terminator will always be appended to the output string.  If the maxCharsToWrite is not large enough, the string will be truncated.
	/// \param[in] input The bitstream containing the compressed string
	/// \param[in] languageID Which language to use
	bool DecodeString( char *output, int maxCharsToWrite, BitStream *input, int languageID=0 );

	/// Used so I can allocate and deallocate this singleton at runtime
	static void AddReference(void);
	
	/// Used so I can allocate and deallocate this singleton at runtime
	static void RemoveReference(void);
	
private:
	
	/// Private Constructor	
	StringCompressor();
	
	/// Singleton instance
	static StringCompressor *instance;
	
	/// Pointer to the huffman encoding tree.
	HuffmanEncodingTree *huffmanEncodingTree;

	static int referenceCount;
};

/// Define to more easily reference the string compressor instance.
/// The RakPeer constructor adds a reference to this class, so don't call this until an instance of RakPeer exists, or unless you call AddReference yourself.
#define stringCompressor StringCompressor::Instance()

#endif
