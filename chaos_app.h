// Main application object for pegasus
//
// Copyright HOLM, 2023

#pragma once

#include "logger.h"

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/function.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/unordered_map.hpp>

#include <set>
#include <string>
#include <unordered_map>

// ===========================================================================
namespace chaos
{
    class ChaosApp
    {
    public:
        ChaosApp(boost::asio::io_service& io, int interval = 1) :
            m_main_io(io),
            m_interval(interval),
            m_timer(io, boost::posix_time::seconds(interval)),
            m_strand(io),
            m_loop(true) 
        {
            m_timer.async_wait(m_strand.wrap(boost::bind(&ChaosApp::boost_timer, this)));
        }

        virtual ~ChaosApp()
        {
            m_timer.cancel();
            m_loop = false;
        }

        virtual void start() = 0;
        virtual void stop() = 0;
        virtual std::string get_version() { return "unknown"; }
        bool loop() { return m_loop; }

        // Returns the main boost io object
        inline boost::asio::io_service* get_main_io() { return &m_main_io; }

    protected:
        virtual void event_loop() = 0;
        void boost_timer()
        {
            // Execute some events
            if (m_loop)
            {
                event_loop();
                // Reset timer
                m_timer.expires_at(m_timer.expires_at() + boost::posix_time::seconds(m_interval));
                m_timer.async_wait(m_strand.wrap(boost::bind(&ChaosApp::boost_timer, this)));
            }
            else
            {
                LOG(PUB_MSG, "PegasusApp thread ending");
            }
        }

        int                             m_interval;
        boost::asio::io_service&        m_main_io;
        boost::asio::deadline_timer     m_timer;
        boost::asio::io_service::strand m_strand;
        volatile bool                   m_loop;

    };

}