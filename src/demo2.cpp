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
#include <opencv2/imgproc.hpp>

// Demo Libraries
#include "Thread_Pool.hpp"

/**
 * @brief Very dumb function which creates a random image, writes to disk, then checks the file
 * size to simulate measuring the compression ratio
*/
void Compress_Imagery( int                                              image_id,
                       cv::Size                                         img_size,
                       const std::filesystem::path&                     dest_dir,
                       acc::Accumulator<acc::FULL_FEATURE_SET,double>&  comp_acc,
                       const std::string&                               ext )
{
    try
    {
        // Create dummy image
        cv::Mat image( img_size, CV_8UC3 );

        const size_t expected_size = img_size.width * img_size.height * 3;

        // Name output path
        std::string pathname = "image_" + std::to_string(image_id) + ext;
        std::filesystem::path output_path = dest_dir / std::filesystem::path( pathname );

        for( size_t r = 0; r < image.rows; r++ )
        for( size_t c = 0; c < image.cols; c++ )
        for( size_t x = 0; x < image.channels(); x++ )
        {
            image.at<cv::Vec3b>( r, c )[x] = rand() % 255;
        }

        // Median filtering will help the compression a bit
        for( int i=0; i<2; i++ )
        {
            cv::medianBlur( image, image, 5 );
        }

        // Write image
        std::vector<int> compression_params;
        if( ext == ".png" )
        {
            compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
            compression_params.push_back(9);
        }
        else if( ext == ".tif" )
        {
            compression_params.push_back(cv::IMWRITE_TIFF_COMPRESSION);
            compression_params.push_back(5);
        }
        cv::imwrite( output_path.c_str(), image, compression_params );

        // Get the original size
        double file_ratio = std::filesystem::file_size( output_path ) / (double)expected_size;
        comp_acc.insert( file_ratio * 100 );

        // Delete the file
        std::filesystem::remove( output_path );
    }
    catch(...)
    {
        std::cout << "Image " << image_id << "Failed to write" << std::endl;
    }
}

bool okay_to_run = true;
void Check_Acc_Status( const acc::Accumulator<acc::FULL_FEATURE_SET, double>& timing_acc,
                       const acc::Accumulator<acc::FULL_FEATURE_SET, double>& compression_acc,
                       bool                                                   single_loop,
                       const std::string&                                     format  )
{
    okay_to_run = true;
    while( okay_to_run )
    {
        std::cout << "Timing Accumulator: " << format << std::endl;
        std::cout << timing_acc.toLogString( ) << std::endl;
        std::cout << "Compression Accumulator" << format << std::endl;
        std::cout << compression_acc.toLogString() << std::endl;
        std::this_thread::sleep_for( std::chrono::seconds( 20 ) );

        if( single_loop )
        {
            break;
        }
    }
}

/**
 * @brief Perform the entire operation on a single image format
*/
void Test_Compression_Format( const std::string& format,
                              int                number_threads )
{
    // General Parameters
    const size_t number_images = 2000;
    const cv::Size image_size ( 1920, 1080 );

    // Create output directory
    std::filesystem::path output_dir("temp");
    if( !std::filesystem::exists( output_dir ) )
    {
        std::filesystem::create_directory( output_dir );
    }

    // Build the two Accumulators
    auto timing_acc      = acc::Accumulator<acc::FULL_FEATURE_SET,double>::create( "ms" );
    auto compression_acc = acc::Accumulator<acc::FULL_FEATURE_SET,double>::create( "%" );

    std::thread status_thread( Check_Acc_Status, std::ref(timing_acc),
                                                 std::ref(compression_acc),
                                                 false,
                                                 std::ref(format) );

    // Create a pile of threads and start the work
    {
        Thread_Pool pool( number_threads );
        pool.init();

        std::deque<std::future<void>> jobs;
        for( size_t worker = 0; worker < number_images; worker++ )
        {
            jobs.push_back( pool.submit([=, &timing_acc, &compression_acc, &format ]() {
                int id = worker;

                acc::Stopwatch<> timer;
                Compress_Imagery( id,
                                  image_size,
                                  output_dir,
                                  compression_acc,
                                  format );
                timing_acc.insert( (double)timer.stop().count() );
            }));
        }

        std::cout << "Waiting for " << format << " jobs to finish" << std::endl;
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
                      true,
                      format );
}

int main( int argc, char* argv[] )
{

    Test_Compression_Format( ".jpg", 3 );
    Test_Compression_Format( ".png", 1 );
    Test_Compression_Format( ".tif", 3 );

    std::cout << "End of Program" << std::endl;
    return 0;
}