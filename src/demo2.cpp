/**
 * @file    demo2.cpp
 * @author  Marvin Smith
 * @date    6/13/2023
 *
 *  Demonstrate using Stopwatch for easier timing.
 *  Use Accumulator for easier metrics management.
*/

// C++ Includes
#include <chrono>
#include <deque>
#include <filesystem>
#include <functional>
#include <iostream>
#include <thread>

// Project Libraries
#include <lib-acc/Accumulator.hpp>
#include <lib-acc/Stopwatch.hpp>

// OpenCV Libraries
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

// Demo Libraries
#include "Thread_Pool.hpp"

/**
 * @brief Very dumb function which creates a random image, writes to disk, then checks the file
 * size to simulate measuring the compression ratio
*/
void Compress_Imagery( int                                              image_id,
                       cv::Size                                         img_size,
                       const std::filesystem::path&                     dest_dir,
                       acc::Accumulator<acc::FULL_FEATURE_SET,double>&  comp_acc )
{
    try
    {
        // Create dummy image
        cv::Mat image( img_size, CV_8UC3 );

        // Name output path
        std::string pathname = "image_" + std::to_string(image_id) + ".jpg";
        std::filesystem::path output_path = dest_dir / std::filesystem::path( pathname );

        for( size_t r = 0; r < image.rows; r++ )
        for( size_t c = 0; c < image.cols; c++ )
        for( size_t x = 0; x < image.channels(); x++ )
        {
            image.at<cv::Vec3b>( r, c )[x] = rand() % 255;
        }

        // Write image
        cv::imwrite( output_path.c_str(), image );

        // Get the original size
        comp_acc.insert( std::filesystem::file_size( output_path )/1000000.0 );

        std::filesystem::remove( output_path );
    }
    catch(...)
    {
        std::cout << "Image " << image_id << "Failed to write" << std::endl;
    }
    std::cout << "Finished Image " << image_id << std::endl;
}

bool okay_to_run = true;
void Check_Acc_Status( const acc::Accumulator<acc::FULL_FEATURE_SET, double>& timing_acc,
                       const acc::Accumulator<acc::FULL_FEATURE_SET, double>& compression_acc,
                       bool                                                   single_loop  )
{
    okay_to_run = true;
    while( okay_to_run )
    {
        std::cout << "Timing Accumulator" << std::endl;
        std::cout << timing_acc.toLogString( ) << std::endl;
        std::cout << "Compression Accumulator" << std::endl;
        std::cout << compression_acc.toLogString() << std::endl;
        std::this_thread::sleep_for( std::chrono::seconds( 5 ) );

        if( single_loop )
        {
            break;
        }
    }
}

int main( int argc, char* argv[] )
{
    // General Parameters
    const size_t number_images = 500;
    const size_t max_threads = 4;
    const cv::Size image_size ( 1920, 1080 );

    // Create output directory
    std::filesystem::path output_dir("temp");
    if( !std::filesystem::exists( output_dir ) )
    {
        std::filesystem::create_directory( output_dir );
    }

    // Build the two Accumulators
    auto timing_acc      = acc::Accumulator<acc::FULL_FEATURE_SET,double>::create( "ms" );
    auto compression_acc = acc::Accumulator<acc::FULL_FEATURE_SET,double>::create( "MB" );

    std::thread status_thread( Check_Acc_Status, std::ref(timing_acc),
                                                 std::ref(compression_acc),
                                                 false );

    // Create a pile of threads and start the work
    {
        Thread_Pool pool( max_threads );

        pool.init();

        std::deque<std::future<void>> jobs;

        // Add each worker to the pool
        for( size_t worker = 0; worker < number_images; worker++ )
        {
            jobs.push_back( pool.submit([=, &timing_acc, &compression_acc]() {
                int id = worker;

                acc::Stopwatch<> timer;
                Compress_Imagery( id,
                                  image_size,
                                  output_dir,
                                  compression_acc );
                timing_acc.insert( (double)timer.stop().count() );
            }));
        }

        std::cout << "Waiting for jobs to finish" << std::endl;
        for( auto& job : jobs ){ job.get(); }

        std::cout << "Shutting down thread pool" << std::endl;
        pool.shutdown();
        std::cout << "Thread pool shut down" << std::endl;
    }

    okay_to_run = false;
    if( status_thread.joinable() )
    {
        status_thread.join();
    }

    // Final printout
    Check_Acc_Status( timing_acc,
                      compression_acc,
                      true );

    std::cout << "End of Program" << std::endl;
    return 0;
}