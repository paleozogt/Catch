/*
 *  catch_runner.hpp
 *  Catch
 *
 *  Created by Phil on 31/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef TWOBLUECUBES_CATCH_RUNNER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_RUNNER_HPP_INCLUDED

#include "internal/catch_commandline.hpp"
#include "internal/catch_list.hpp"
#include "catch_reporter_basic.hpp"
#include "catch_reporter_xml.hpp"

namespace Catch
{
    inline int Main( int argc, char * const argv[] )
    {
        ReporterRegistry::instance().registerReporter<BasicReporter>( "basic" );
        ReporterRegistry::instance().registerReporter<XmlReporter>( "xml" );

        RunnerConfig config;
        ArgParser( argc, argv, config );     
        
        if( !config.m_message.empty() )
        {
            std::cerr << config.m_message << std::endl;
            return std::numeric_limits<int>::max();
        }
        
        // Handle list request
        if( config.listWhat() != RunnerConfig::listNone )
            return List( config );
        
        // Open output file, if specified
        std::ofstream ofs;
        if( !config.getFilename().empty() )
        {
            ofs.open( config.getFilename().c_str() );
            if( ofs.fail() )
            {
                std::cerr << "Unable to open file: '" << config.getFilename() << "'" << std::endl;
                return std::numeric_limits<int>::max();
            }
            config.getReporterConfig().setStreamBuf( ofs.rdbuf() );
        }

        Runner runner;
        runner.setReporter( config.getReporter() );

        // Run test specs specified on the command line - or default to all
        if( config.m_testSpecs.size() == 0 )
        {
            runner.runAll();
        }
        else
        {
            // !TBD We should get all the testcases upfront, report any missing,
            // then just run them
            std::vector<std::string>::const_iterator it = config.m_testSpecs.begin();
            std::vector<std::string>::const_iterator itEnd = config.m_testSpecs.end();
            for(; it != itEnd; ++it )
            {
                if( runner.runMatching( *it ) == 0 )
                {
                    // Use reporter?
                    std::cerr << "\n[Unable to match any test cases with: " << *it << "]" << std::endl;
                }
            }
        }
        return runner.getFailures();
    }
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RUNNER_HPP_INCLUDED