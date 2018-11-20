#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <boost/icl/interval_set.hpp>
#include <boost/icl/separate_interval_set.hpp>
#include <boost/icl/split_interval_set.hpp>
#include <boost/icl/split_interval_map.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost::icl;

// Range structure to store given ranges in start and end manner
typedef struct Range
{
    Range( string start_, string end_ )
        : start( start_ )
        , end( end_ )
    {
    }
    string start;
    string end;
} Range;

// Split function to distinguish white space and stores in vector
void
split( string& str, vector< string >& tokens )
{
    char* token = strtok( (char*)str.c_str( ), " " );
    while ( token != NULL )
    {
        tokens.push_back( token );
        token = strtok( NULL, " " );
    }
}

/******************************************************
 * process_str function which does the actual processing
 * Input:
 * str: Main string
 * interval set: which contains the ranges
 * bytes: bool variable
 * complement : bool varaible
 * result: for output string
 * **********************************************************/
void
process_str( string& str, interval_set< int >& iset, bool bytes, bool complement, string& result )
{
    vector< string > strList;
    if ( !bytes )
    {
        split( str, strList );
    }
    int size = bytes ? str.size( ) - 1 : strList.size( ) - 1;
    if ( complement )
    {
        interval_set< int > cset;
        cset += interval< int >::closed( 0, size );
        iset = cset - iset;
    }
    // cout<<"Ranges "<<iset<<"\n";

    for ( auto it = iset.begin( ); it != iset.end( ); ++it )
    {
        // to check interval is a closed interval or open interval
        int lower = contains( *it, it->lower( ) ) ? it->lower( ) : it->lower( ) + 1;
        int upper = contains( *it, it->upper( ) ) ? it->upper( ) : it->upper( ) - 1;
        for ( int i = lower; i <= upper && i <= size; i++ )
        {
            if ( bytes )
            {
                result += str[ i ];
            }
            else
            {
                result += strList[ i ];
                result += " ";
            }
        }
    }

    if ( !bytes )
    {
        boost::trim_right( result );
    }
    result += "\n";
}
/******************************************************
 * Select function which calls the actual process_str function
 * Input:
 * str: Main string
 * ranges: vector of given ranges set by user
 * bytes: bool variable
 * complement : bool varaible
 * output:
 * return a string
 * **********************************************************/
string
select( string str, vector< Range > ranges, bool bytes, bool complement )
{
    string result = "";
    std::stringstream iss( str );

    while ( iss.good( ) )
    {
        std::string line;
        getline( iss, line, '\n' );
        if ( line.length( ) == 0 )
        {
            // no need to process this
            return result;
        }
        interval_set< int > set;
        int start = 0;
        int end = 0;
        // if '-' sysmbol is in beginning then replace by 0
        // if '-' symbol found in end replace by line length
        for ( const auto& it : ranges )
        {
            start = it.start == "-" ? 0 : atoi( it.start.c_str( ) );
            if ( !bytes )
            {
                int space_seperated_len = 0;
                for ( unsigned int x = 0; x < line.length( ); x++ )
                {
                    if ( line[ x ] == ' ' )
                    {
                        space_seperated_len++;
                    }
                }
                end = it.end == "-" ? space_seperated_len : atoi( it.end.c_str( ) );
            }
            else
                end = it.end == "-" ? line.length( ) : atoi( it.end.c_str( ) );

            set += interval< int >::closed( start, end );
        }
        process_str( line, set, bytes, complement, result );
    }
    return result;
}

/*************************************************
 
 * Here I am passing the desired output and checking with the end result,
 * if string matches test case is passed otherwise failed.
 *
 ****************************************************/
bool
test_Case_one( )
{
    string str = "abcdefghijklmnopqrstuvwxyz\n";
    string desired_output = "bcfgh\n";
    vector< Range > ranges;
    ranges.push_back( Range( "1", "2" ) );
    ranges.push_back( Range( "5", "7" ) );

    string result = select( str, ranges, true, false );
    cout << result << endl;
    if ( result.size( ) > 0 )
    {
        if ( result.compare( desired_output ) == 0 )
        {
            cout << "Test Is Passed" << endl;
            cout << "___________________Test End()_____________" << endl;
            return true;
        }
    }
    else
    {
        cout << "Test Is Failed" << endl;
    }
    return false;
}
bool
test_Case_two( )
{
    string str = "abcdefghijklmnopqrstuvwxyz\n";
    string desired_output = "adeijklmnopqrstuvwxyz\n";
    vector< Range > ranges;
    ranges.push_back( Range( "1", "2" ) );
    ranges.push_back( Range( "5", "7" ) );

    string result = select( str, ranges, true, true );
    cout << result << endl;
    if ( result.size( ) > 0 )
    {
        if ( result.compare( desired_output ) == 0 )
        {
            cout << "Test Is Passed" << endl;
            cout << "___________________Test End()_____________" << endl;
            return true;
        }
    }
    else
    {
        cout << "Test Is Failed" << endl;
    }
    return false;
}

bool
test_Case_three( )
{
    string str = "abcdefghijklmnopqrstuvwxyz\n";
    string desired_output = "bcdefg\n";
    vector< Range > ranges;
    ranges.push_back( Range( "1", "4" ) );
    ranges.push_back( Range( "3", "6" ) );

    string result = select( str, ranges, true, false );
    cout << result << endl;
    if ( result.size( ) > 0 )
    {
        if ( result.compare( desired_output ) == 0 )
        {
            cout << "Test Is Passed" << endl;
            cout << "___________________Test End()_____________" << endl;
            return true;
        }
    }
    else
    {
        cout << "Test Is Failed" << endl;
    }
    return false;
}

bool
test_Case_four( )
{
    string str = "This is a longer string\nsplit over two lines\n";
    string desired_output = "This a longer string\nsplit two lines\n";
    vector< Range > ranges;
    ranges.push_back( Range( "0", "0" ) );
    ranges.push_back( Range( "2", "-" ) );

    string result = select( str, ranges, false, false );
    cout << result << endl;
    if ( result.size( ) > 0 )
    {
        if ( result.compare( desired_output ) == 0 )
        {
            cout << "Test Is Passed" << endl;
            cout << "___________________Test End()_____________" << endl;
            return true;
        }
    }
    else
    {
        cout << "Test Is Failed" << endl;
    }
    return false;
}
bool
test_Case_five( )
{
    string str = "This is a longer string\nsplit over two lines\n";
    string desired_output = "is\nover\n";
    vector< Range > ranges;
    ranges.push_back( Range( "0", "0" ) );
    ranges.push_back( Range( "2", "-" ) );

    string result = select( str, ranges, false, true );
    cout << result << endl;
    if ( result.size( ) > 0 )
    {
        if ( result.compare( desired_output ) == 0 )
        {
            cout << "Test Is Passed" << endl;
            cout << "___________________Test End()_____________" << endl;
            return true;
        }
    }
    else
    {
        cout << "Test Is Failed" << endl;
    }
    return false;
}
bool
test_Case_six( )
{
    string str = "This is a longer string\nsplit over two lines\n";
    string desired_output = "This is a string\nsplit over two\n";
    vector< Range > ranges;
    ranges.push_back( Range( "-", "2" ) );
    ranges.push_back( Range( "4", "-" ) );

    string result = select( str, ranges, false, false );
    cout << result << endl;
    if ( result.size( ) > 0 )
    {
        if ( result.compare( desired_output ) == 0 )
        {
            cout << "Test Is Passed" << endl;
            cout << "___________________Test End()_____________" << endl;
            return true;
        }
    }
    else
    {
        cout << "Test Is Failed" << endl;
    }
    return false;
}
int
main( )
{
    test_Case_one( );
    test_Case_two( );
    test_Case_three( );
    test_Case_four( );
    test_Case_five( );
    test_Case_six( );

    return 0;
}
