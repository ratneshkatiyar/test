#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <queue>
#include <functional>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

using namespace std;
typedef pair< int, int > pii;
typedef std::function< void( ) > Func;

// , you can adjust times to run all three threads parallely
void
fun( void )
{
    cout << "Having fun! & thread ID " << std::this_thread::get_id( ) << endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
}

void
fun1( void )
{
    cout << "Having fun1! & thread ID  " << std::this_thread::get_id( ) << endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
}
void
fun2( void )
{
    cout << "Having fun2! & thread ID  " << std::this_thread::get_id( ) << endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
}
struct task
{
    // priority number
    task( int _id, Func _f )
        : id( _id )
        , status( task::queued )
        , f( _f )
    {
    }
    task( )
    {
    }

    // Priority number
    int id;
    // task stage
    enum
    {
        queued = 0,
        running,
        finished
    } status;

    // function to call for
    Func f;

    void
    operator( )( )
    {
        f( );
    }
};

struct Comp
{
    bool
    operator( )( const task& a, const task& b )
    {
        return a.id > b.id;
    }
};

class ThreadPool
{
public:
    ThreadPool( )
        : m_thOne( [this] { call_from_thread( ); } )
        , m_thTwo( [this] { call_from_thread( ); } )
        , m_thThree( [this] { call_from_thread( ); } )
        , m_jobDone( false )
    {
    }
    std::thread m_thOne;
    std::thread m_thTwo;
    std::thread m_thThree;

    void InsertNewTaskInQueue( task t );

private:
    void call_from_thread( );

private:
    bool m_jobDone;

    std::mutex m_mu;
    std::condition_variable m_cv;

public:
    priority_queue< struct task, vector< task >, Comp > m_pq;
};

void
ThreadPool::InsertNewTaskInQueue( task t )
{
    {
        std::lock_guard< std::mutex > lock( m_mu );
        m_pq.push( t );
    }
    m_cv.notify_all( );
}
void
ThreadPool::call_from_thread( )
{
    while ( !m_jobDone )
    {
        task temp;
        bool do_we_have_job = false;
        {
            std::unique_lock< std::mutex > lck( m_mu );
            if ( !m_pq.empty( ) )
            {
                temp = m_pq.top( );
                m_pq.pop( );
                do_we_have_job = true;
            }
            else
            {
                //
                m_cv.wait( lck );
            }
        }

        if ( do_we_have_job )
        {
            try
            {
                // cout << "ID to execute is: " << temp.id << endl;
                temp.status = task::running;
                cout << "status of Current running task n priority number are : " << temp.status
                     << "  " << temp.id << endl;
                temp.f( );
                temp.status = task::finished;
                cout << "status of Current running task n priority number are : " << temp.status
                     << "  " << temp.id << endl;
            }
            catch ( ... )
            {
                cout << "Exception caught" << endl;
            }
        }
        else
        {
            continue;
        }
    }
}

/*******************************************TestCases******************************************************/

/*************************************************************************************************************/
typedef priority_queue< struct task, vector< task >, Comp > PriorityQueue;
bool
testCaseOne( const PriorityQueue& _pq )
{
    // checking the task that must be in highest to lowest priority otherwise return false

    PriorityQueue pq = _pq;
    if ( !pq.empty( ) )
    {
        task temp_f = pq.top( );
        while ( !pq.empty( ) )
        {
            pq.pop( );
            task temp_s = pq.top( );
            if ( temp_s.id < temp_f.id )
            {
                cout << "Test Case Failed" << endl;
                return false;
            }
        }
        cout << "Test Case Passed: " << endl;
        return true;
    }

    return false;
}

void
testCaseTwo( )
{  
}
/***********************************************************************************************************/
int
main( )
{
    ThreadPool tp;

    task t( 50, fun );

    task t1( 10, fun1 );
    task t2( 70, fun2 );
    tp.InsertNewTaskInQueue( t );
    tp.InsertNewTaskInQueue( t2 );
    tp.InsertNewTaskInQueue( t1 );

    testCaseOne( tp.m_pq );

    std::this_thread::sleep_for( std::chrono::milliseconds( 10000 ) );
    // after some time a new task is ready to execute
    task t3( 100, fun2 );

    tp.InsertNewTaskInQueue( t3 );
    // would be better to create a vector<std::thread >
    tp.m_thOne.join( );
    tp.m_thTwo.join( );
    tp.m_thThree.join( );
    return 0;
}

/**************************Output*****************************************
Test Case Passed:
status of Current running task n priority number are : 1  10
Having fun1! & thread ID  140202148669184
status of Current running task n priority number are : 1  50
Having fun! & thread ID 140202157061888
status of Current running task n priority number are : 1  70
Having fun2! & thread ID  140202140276480
/*********************************************************************/
