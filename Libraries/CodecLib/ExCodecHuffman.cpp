/*
 *  ExCodecHuffman.cpp
 *  Exeter Codec Library
 *
 *  Created by Tom Weatherhead on Fri Aug 01 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifdef _MSC_VER			// TAW 2002/03/11 : ie. if MSVC
#ifndef MACINTOSH		// TAW 2002/02/23 : ie. if MSVC

// Suppress warning C4786: Identifier truncated to 255 chars in the debug info.
#pragma warning(disable : 4786)

#endif
#endif


//#define TAW_TRACE		1
#define TAW_HUFFMAN_TEST		1

#include <map>
#include <queue>

#ifdef TAW_HUFFMAN_TEST
#include <cstdio>
#endif

#include "General\\ExAPAutoPtr.h"
#include "General\\ExAPException.h"
#include "General\\ExAPMinMax.h"
//#include "CodecLib\\ExCodecBitByteSourceSink.h"
#include "CodecLib\\ExCodecHuffman.h"
#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"

#ifdef TAW_HUFFMAN_TEST
#include "ExCodecTestBitHolder.h"
#endif


// **** Class ExCodecDataSource ****


template<class C> class ExCodecDataSource
{
public:

	virtual ~ExCodecDataSource( void )
	{
	}

	virtual void Rewind( void ) = 0;	// Return to start of data for a subsequent pass.
	virtual C GetData( void ) = 0;		// Return data by value.
	virtual bool EndOfData( void ) = 0;
}; // template<class C> class ExCodecDataSource


// **** Class ExCodecDataSink ****


template<class C> class ExCodecDataSink
{
public:

	virtual ~ExCodecDataSink( void )
	{
	}

	virtual void WriteData( const C & datum ) = 0;
}; // template<class C> class ExCodecDataSink


// **** Class ExCodecHuffmanHistogram ****


template<class C> class ExCodecHuffmanHistogram
{
public:
	typedef std::map<C, unsigned int> MapType;

private:
	MapType m_map;

public:

	ExCodecHuffmanHistogram( ExCodecDataSource<C> & dataSource )
	{
#ifdef TAW_TRACE
		unsigned int srcDataCount = 0;
#endif

		for( ; ; )
		{
			const C datum = dataSource.GetData();

			if( dataSource.EndOfData() )
			{
				break;
			}

#ifdef TAW_TRACE
			++srcDataCount;
#endif

			MapType::iterator it = m_map.find( datum );

			if( it == m_map.end() )
			{
				m_map[datum] = 1;
			}
			else
			{
				//m_map[datum]++;
				it->second++;
			}
		}

#ifdef TAW_TRACE
		printf( "Histogram: %u data objects.\n", srcDataCount );
#endif
	}

	inline MapType::const_iterator GetBeginIterator( void ) const
	{
		return( m_map.begin() );
	}

	inline MapType::const_iterator GetEndIterator( void ) const
	{
		return( m_map.end() );
	}
}; // template<class C> class ExCodecHuffmanHistogram


// **** Class ExCodecHuffmanBitString ****


class ExCodecHuffmanBitString
{
private:
	std::vector<unsigned int> m_v;
	unsigned int m_numBits;

public:
	ExCodecHuffmanBitString( void );

	// operator < is needed for this class to be the key to a std::map.
	bool operator <( const ExCodecHuffmanBitString & src ) const;

	void Clear( void );

	void AppendBit( bool bit );

	void Write( ExAPAutoPtr<ExUtilBitSinkInterface> sink ) const;

#ifdef TAW_TRACE
	void Print( void ) const;
#endif
}; // class ExCodecHuffmanBitString


ExCodecHuffmanBitString::ExCodecHuffmanBitString( void )
	: m_numBits( 0 )
{
} // ExCodecHuffmanBitString::ExCodecHuffmanBitString()


bool ExCodecHuffmanBitString::operator <( const ExCodecHuffmanBitString & src ) const
{
	return( m_numBits < src.m_numBits  ||
		( m_numBits == src.m_numBits  &&  m_v < src.m_v ) );
} // ExCodecHuffmanBitString::operator <()


void ExCodecHuffmanBitString::Clear( void )
{
	m_v.clear();
	m_numBits = 0;
} // ExCodecHuffmanBitString::Clear()


void ExCodecHuffmanBitString::AppendBit( bool bit )
{
	// Assume 8 bits per byte.
	//static const unsigned int numBitsInUInt = 8 * sizeof( unsigned int );

	if( m_v.empty() )
	{
		m_v.push_back( 0 );
	}

	unsigned int & back = m_v.back();

	const unsigned int mask = (unsigned int)( 1 << ( 31 - m_numBits % 32 ) );

	if( bit )
	{
		back |= mask;
	}

	++m_numBits;
} // ExCodecHuffmanBitString::AppendBit()


void ExCodecHuffmanBitString::Write( ExAPAutoPtr<ExUtilBitSinkInterface> sink ) const
{
	// Write 16 bits at a time to avoid overflowing the sink.
	std::vector<unsigned int>::const_iterator it = m_v.begin();
	int numBitsLeft = int( m_numBits );
	bool writeHighOrderBits = true;

	while( numBitsLeft > 0 )
	{
		int bits = 0;
		const int numBitsToWriteThisIteration = min( 16, numBitsLeft );

		if( writeHighOrderBits )
		{
			bits = int( *it >> ( 32 - numBitsToWriteThisIteration ) );
		}
		else
		{
			bits = int( *it++ >> ( 16 - numBitsToWriteThisIteration ) );
		}

		sink->WriteBits( bits, numBitsToWriteThisIteration );
		writeHighOrderBits = !writeHighOrderBits;
		numBitsLeft -= numBitsToWriteThisIteration;
	}
} // ExCodecHuffmanBitString::Write()


#ifdef TAW_TRACE
void ExCodecHuffmanBitString::Print( void ) const
{
	unsigned int idx = 0;
	unsigned int currentUInt = 0;
	unsigned int mask = 0;

	for( unsigned int i = 0; i < m_numBits; ++i )
	{

		if( i % 32 == 0 )
		{
			currentUInt = m_v[i / 32];
			mask = (unsigned int)( 1 << 31 );
		}

		printf( "%c", ( ( currentUInt & mask ) != 0 ) ? '1' : '0' );
		mask >>= 1;
	}
} // ExCodecHuffmanBitString::Print()
#endif


// **** Class ExCodecHuffmanTreeNode ****


template<class C> class ExCodecHuffmanTreeNode
{
protected:
	const unsigned int m_population;

public:

	explicit ExCodecHuffmanTreeNode( unsigned int population );

	inline unsigned int GetPopulation( void ) const
	{
		return( m_population );
	}

	virtual void BuildHuffmanCodes(
		std::map<C, ExCodecHuffmanBitString> & dictionary,
		const ExCodecHuffmanBitString & bitString
		) const = 0;
}; // class ExCodecHuffmanTreeNode


template<class C> ExCodecHuffmanTreeNode<C>::ExCodecHuffmanTreeNode( unsigned int population )
	: m_population( population )
{
} // ExCodecHuffmanTreeNode::ExCodecHuffmanTreeNode()


// **** Class ExCodecHuffmanTreeNonleaf ****


template<class C> class ExCodecHuffmanTreeNonleaf : public ExCodecHuffmanTreeNode<C>
{
private:
	const ExAPAutoPtr< ExCodecHuffmanTreeNode<C> > m_leftSubtree;
	const ExAPAutoPtr< ExCodecHuffmanTreeNode<C> > m_rightSubtree;

public:

	ExCodecHuffmanTreeNonleaf(
		ExAPAutoPtr< ExCodecHuffmanTreeNode<C> > leftSubtree,
		ExAPAutoPtr< ExCodecHuffmanTreeNode<C> > rightSubtree );

	virtual void BuildHuffmanCodes(
		std::map<C, ExCodecHuffmanBitString> & dictionary,
		const ExCodecHuffmanBitString & bitString ) const;
}; // class ExCodecHuffmanTreeNonleaf


template<class C> ExCodecHuffmanTreeNonleaf<C>::ExCodecHuffmanTreeNonleaf(
	ExAPAutoPtr< ExCodecHuffmanTreeNode<C> > leftSubtree,
	ExAPAutoPtr< ExCodecHuffmanTreeNode<C> > rightSubtree )
	: ExCodecHuffmanTreeNode<C>( leftSubtree->GetPopulation() + rightSubtree->GetPopulation() ),
		m_leftSubtree( leftSubtree ),
		m_rightSubtree( rightSubtree )
{
} // ExCodecHuffmanTreeNonleaf::ExCodecHuffmanTreeNonleaf()


template<class C> void ExCodecHuffmanTreeNonleaf<C>::BuildHuffmanCodes(
	std::map<C, ExCodecHuffmanBitString> & dictionary,
	const ExCodecHuffmanBitString & bitString ) const
{
	// Traverse the left subtree, adding a zero bit.
	ExCodecHuffmanBitString leftBitString( bitString );

	leftBitString.AppendBit( false );
	m_leftSubtree->BuildHuffmanCodes( dictionary, leftBitString );

	// Traverse the right subtree, adding a one bit.
	ExCodecHuffmanBitString rightBitString( bitString );

	rightBitString.AppendBit( true );
	m_rightSubtree->BuildHuffmanCodes( dictionary, rightBitString );
} // ExCodecHuffmanTreeNonleaf::BuildHuffmanCodes()


// **** Class ExCodecHuffmanTreeLeaf ****


template<class C> class ExCodecHuffmanTreeLeaf : public ExCodecHuffmanTreeNode<C>
{
private:
	const C m_value;

public:

	ExCodecHuffmanTreeLeaf( const C & value, unsigned int population )
		: ExCodecHuffmanTreeNode<C>( population ),
			m_value( value )
	{
	}

	virtual void BuildHuffmanCodes(
		std::map<C, ExCodecHuffmanBitString> & dictionary,
		const ExCodecHuffmanBitString & bitString ) const
	{
#ifdef TAW_TRACE
		const int valueAsInt = int( m_value );

		printf( "Assigning code " );
		bitString.Print();

		if( valueAsInt == 10  ||  valueAsInt == 13 )
		{
			printf( " to unprintable char (%d).\n", valueAsInt );
		}
		else
		{
			printf( " to char '%c' (%d).\n", char( m_value ), valueAsInt );
		}
#endif

		dictionary[m_value] = bitString;
	}
}; // class ExCodecHuffmanTreeLeaf


// **** Class ExCodecHuffmanTreeNodeComparator ****


template<class C> class ExCodecHuffmanTreeNodeComparator
{
public:
	// This class is used to order the subtrees in the priority queue.

	bool operator()( const ExAPAutoPtr< ExCodecHuffmanTreeNode<C> > & node1, const ExAPAutoPtr< ExCodecHuffmanTreeNode<C> > & node2 )
	{
		return( node1->GetPopulation() > node2->GetPopulation() );
	}
}; // class ExCodecHuffmanTreeNodeComparator


// **** Class ExCodecHuffman ****


template<class C> class ExCodecHuffman
{
private:
	std::map<C, ExCodecHuffmanBitString> m_dictionary;
	ExCodecDataSource<C> & m_dataSource;
	ExAPAutoPtr<ExUtilBitSinkInterface> m_bitSink;
	unsigned int m_totalSrcPopulation;

public:

	ExCodecHuffman( ExCodecDataSource<C> & dataSource, ExAPAutoPtr<ExUtilBitSinkInterface> bitSink )
		: m_dataSource( dataSource ),
			m_bitSink( bitSink ),
			m_totalSrcPopulation( 0 )
	{
	}

	void BuildDictionary( void );
	void EncodeData( void );
	void DecodeData( ExAPAutoPtr<ExUtilBitSourceInterface> source, ExCodecDataSink<C> & sink );

#ifdef TAW_TRACE
	void PrintDictionary( void ) const;
#endif
}; // class ExCodecHuffman


template<class C> void ExCodecHuffman<C>::BuildDictionary( void )
{
	const ExCodecHuffmanHistogram<C> histogram( m_dataSource );
	std::priority_queue<
		ExAPAutoPtr<ExCodecHuffmanTreeNode<C> >,
		std::vector< ExAPAutoPtr<ExCodecHuffmanTreeNode<C> > >,
		ExCodecHuffmanTreeNodeComparator<C> > priorityQ;

	// Populate the priority queue.
	ExCodecHuffmanHistogram<C>::MapType::const_iterator histIt = histogram.GetBeginIterator();
#ifdef TAW_TRACE
	unsigned int histItCount = 0;
#endif

	while( histIt != histogram.GetEndIterator() )
	{
		const C & datum = histIt->first;
		const unsigned int population = histIt->second;
		ExCodecHuffmanTreeNode<C> * leafRaw = new ExCodecHuffmanTreeLeaf<C>( datum, population );
		ExAPAutoPtr< ExCodecHuffmanTreeNode<C> > leaf = leafRaw;

#ifdef TAW_TRACE
		histItCount += population;
#endif

		priorityQ.push( leaf );
		++histIt;
	}

#ifdef TAW_TRACE
	printf( "Iterated histogram population: %u\n", histItCount );
#endif

	ExAssert( !priorityQ.empty() )

	// Build the Huffman tree.
	ExAPAutoPtr< ExCodecHuffmanTreeNode<C> > root;

	for( ; ; )
	{
		ExAPAutoPtr< ExCodecHuffmanTreeNode<C> > node1 = priorityQ.top();

		priorityQ.pop();

		if( priorityQ.empty() )
		{
			// The tree is complete.
			root = node1;
#ifdef TAW_TRACE
			printf( "HuffmanTree: Root has pop %u.\n", root->GetPopulation() );
#endif
			break;
		}

		ExAPAutoPtr< ExCodecHuffmanTreeNode<C> > node2 = priorityQ.top();

		priorityQ.pop();

#ifdef TAW_TRACE
		printf( "HuffmanTree: Combining subtrees of pop %u and %u.\n", node1->GetPopulation(), node2->GetPopulation() );
#endif

		ExCodecHuffmanTreeNode<C> * newNodeRaw = new ExCodecHuffmanTreeNonleaf<C>( node1, node2 );
		ExAPAutoPtr< ExCodecHuffmanTreeNode<C> > newNode = newNodeRaw;

		priorityQ.push( newNode );
	}

	m_totalSrcPopulation = root->GetPopulation();

	// Build the Huffman dictionary.
	root->BuildHuffmanCodes( m_dictionary, ExCodecHuffmanBitString() );
} // ExCodecHuffman::BuildDictionary()


template<class C> void ExCodecHuffman<C>::EncodeData( void )
{
	m_dataSource.Rewind();

	for( ; ; )
	{
		const C srcDatum = m_dataSource.GetData();

		if( m_dataSource.EndOfData() )
		{
			break;
		}

		const ExCodecHuffmanBitString & code = m_dictionary[srcDatum];

		code.Write( m_bitSink );
	}

	m_bitSink->Flush();
} // ExCodecHuffman::EncodeData()


template<class C> void ExCodecHuffman<C>::DecodeData( ExAPAutoPtr<ExUtilBitSourceInterface> source, ExCodecDataSink<C> & sink )
{
	// Build the decode dictionary.
	std::map<ExCodecHuffmanBitString, C> decodeDictionary;
	std::map<C, ExCodecHuffmanBitString>::const_iterator it = m_dictionary.begin();
	const std::map<C, ExCodecHuffmanBitString>::const_iterator itEnd = m_dictionary.end();

	while( it != itEnd )
	{
		decodeDictionary[it->second] = it->first;
		++it;
	}

	ExCodecHuffmanBitString bitString;
	unsigned int numObjectsToWrite = m_totalSrcPopulation;

	while( numObjectsToWrite > 0 )
	{
		// Reads one bit at a time.  This will be brutally slow.
		const bool srcBit = source.ReadBits( 1 ) != 0;

		if( source.EndOfFile() )
		{
#ifdef TAW_TRACE
			printf( "Source EOF found.\n" );
#endif
			break;
		}

#if 0 //def TAW_TRACE
		printf( "Read bit %c.\n", srcBit ? '1' : '0' );
#endif

		bitString.AppendBit( srcBit );

		const std::map<ExCodecHuffmanBitString, C>::const_iterator itDecode = decodeDictionary.find( bitString );

		if( itDecode != decodeDictionary.end() )
		{
#if 0 //def TAW_TRACE
			printf( "Found code '" );
			bitString.Print();
			printf( "' in decode dictionary.\n" );
#endif

#ifdef TAW_TRACE
			printf( "%c", char( itDecode->second ) );
#endif
			sink.WriteData( itDecode->second );
			--numObjectsToWrite;
			bitString.Clear();
		}
	}

#ifdef TAW_TRACE
	printf( "\n" );
#endif
} // ExCodecHuffman::DecodeData()


#ifdef TAW_TRACE
template<class C> void ExCodecHuffman<C>::PrintDictionary( void ) const
{
	std::map<C, ExCodecHuffmanBitString>::const_iterator it = m_dictionary.begin();
	const std::map<C, ExCodecHuffmanBitString>::const_iterator itEnd = m_dictionary.end();

	while( it != itEnd )
	{
		const int firstAsInt = int( it->first );

		it->second.Print();

		if( firstAsInt == 10  ||  firstAsInt == 13 )
		{
			printf( ": Unprintable (%d)\n", firstAsInt );
		}
		else
		{
			printf( ": '%c' (%d)\n", char( it->first ), firstAsInt );
		}

		++it;
	}
} // ExCodecHuffman::PrintDictionary()
#endif


#ifdef TAW_HUFFMAN_TEST


// **** Class ExCodecHuffmanTestByteSource ****


class ExCodecHuffmanTestByteSource : public ExCodecDataSource<unsigned char>
{
private:
	FILE * m_fp;

public:
	ExCodecHuffmanTestByteSource( const std::string & srcFilename );
	virtual ~ExCodecHuffmanTestByteSource( void );
	virtual void Rewind( void );		// Return to start of data for a subsequent pass.
	virtual unsigned char GetData( void );			// Return data by value.
	virtual bool EndOfData( void );
}; // class ExCodecHuffmanTestByteSource


ExCodecHuffmanTestByteSource::ExCodecHuffmanTestByteSource( const std::string & srcFilename )
	: m_fp( fopen( srcFilename.c_str(), "rb" ) )
{
	ExAssert( m_fp != 0 )
} // ExCodecHuffmanTestByteSource::ExCodecHuffmanTestByteSource()


ExCodecHuffmanTestByteSource::~ExCodecHuffmanTestByteSource( void )
{

	if( m_fp != 0 )
	{
		fclose( m_fp );
		m_fp = 0;
	}
} // ExCodecHuffmanTestByteSource::~ExCodecHuffmanTestByteSource()


void ExCodecHuffmanTestByteSource::Rewind( void )
{
	const int status = fseek( m_fp, 0L, SEEK_SET );

	ExAssert( status == 0 )
} // ExCodecHuffmanTestByteSource::Rewind()


unsigned char ExCodecHuffmanTestByteSource::GetData( void )
{
	return( (unsigned char)fgetc( m_fp ) );
} // ExCodecHuffmanTestByteSource::GetData()


bool ExCodecHuffmanTestByteSource::EndOfData( void )
{
	return( feof( m_fp ) != 0 );
} // ExCodecHuffmanTestByteSource::EndOfData()


// **** Class ExCodecHuffmanTestByteSink ****


class ExCodecHuffmanTestByteSink : public ExCodecDataSink<unsigned char>
{
private:
	FILE * m_fp;

public:
	ExCodecHuffmanTestByteSink( const std::string & dstFilename );
	virtual ~ExCodecHuffmanTestByteSink( void );
	virtual void WriteData( const unsigned char & datum );
}; // class ExCodecHuffmanTestByteSink


ExCodecHuffmanTestByteSink::ExCodecHuffmanTestByteSink( const std::string & dstFilename )
	: m_fp( fopen( dstFilename.c_str(), "wb" ) )
{
	ExAssert( m_fp != 0 )
} // ExCodecHuffmanTestByteSink::ExCodecHuffmanTestByteSink()


ExCodecHuffmanTestByteSink::~ExCodecHuffmanTestByteSink( void )
{

	if( m_fp != 0 )
	{
		fclose( m_fp );
		m_fp = 0;
	}
} // ExCodecHuffmanTestByteSink::~ExCodecHuffmanTestByteSink()


void ExCodecHuffmanTestByteSink::WriteData( const unsigned char & datum )
{
	const int status = fputc( int( datum ), m_fp );

	ExAssert( status != EOF )
} // ExCodecHuffmanTestByteSink::WriteData()


// **** Non-member functions ****


void HuffmanEncodingTest( const std::string & srcFilename )
{
	ExCodecHuffmanTestByteSource source( srcFilename );
	ExCodecHuffmanTestBitHolder * bitHolder = new ExCodecHuffmanTestBitHolder;
	ExUtilBitSinkInterface * bitHolderSinkRaw = bitHolder;
	ExAPAutoPtr<ExUtilBitSinkInterface> bitHolderSink = bitHolderSinkRaw;
	ExCodecHuffman<unsigned char> codec( source, bitHolder );

	codec.BuildDictionary();

#if 0 //def TAW_TRACE
	codec.PrintDictionary();
#endif

	codec.EncodeData();

#if 0
	{
		const std::string dstFilename = "HuffTestOut.txt";
		ExCodecHuffmanTestByteSink sink( dstFilename );
		
		bitHolder.PrepareToRead();

#ifdef TAW_TRACE
		printf( "Decoding...\n" );
#endif

		codec.DecodeData( bitHolder, sink );

#ifdef TAW_TRACE
		printf( "Finished decoding.\n" );
#endif
	}
#endif

#ifdef TAW_TRACE
	printf( "End of HuffmanEncodingTest.\n" );
#endif
} // HuffmanEncodingTest()


#endif // #ifdef TAW_HUFFMAN_TEST


// **** End of File ****
