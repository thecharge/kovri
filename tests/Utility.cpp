#include <boost/test/unit_test.hpp>
#include "util/HTTP.h"

BOOST_AUTO_TEST_SUITE(UtilityTests)

using namespace i2p::util::http;

BOOST_AUTO_TEST_CASE(DecodeEmptyUrl)
{
    BOOST_CHECK_EQUAL(urlDecode(""), "");
}

BOOST_AUTO_TEST_CASE(DecodeUrl)
{
    BOOST_CHECK_EQUAL(urlDecode("%20"), " ");
}

BOOST_AUTO_TEST_CASE(ParseHTTPRequestNoHeaders)
{
    Request req1("GET /index.html HTTP/1.1");
    Request req2("POST / HTTP/1.0\r\n");
    BOOST_CHECK_EQUAL(req1.getMethod(), "GET");
    BOOST_CHECK_EQUAL(req1.getUri(), "/index.html");
    BOOST_CHECK_EQUAL(req2.getMethod(), "POST");
    BOOST_CHECK_EQUAL(req2.getUri(), "/");
}

BOOST_AUTO_TEST_CASE(ParseHTTPRequestWithHeaders)
{
    Request req1(
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
    );
    Request req2(
        "POST / HTTP/1.1\r\n"
        "Host: localhost:123        \r\n"
    );
    BOOST_CHECK_EQUAL(req1.getHeader("Host"), "localhost");
    BOOST_CHECK_EQUAL(req2.getHeader("Host"), "localhost:123");
}

BOOST_AUTO_TEST_CASE(ParseHTTPRequestWithContent)
{
    Request req1(
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n\r\n"
        "Random content."
    );
    Request req2(
        "GET /index.html HTTP/1.0\r\n\r\n"
        "Random content.\r\nTest content."
    );
    BOOST_CHECK_EQUAL(req1.getContent(), "Random content.");
    BOOST_CHECK_EQUAL(req2.getContent(), "Random content.\r\nTest content.");
}

BOOST_AUTO_TEST_CASE(ParseHTTPRequestWithPartialHeaders)
{
    Request req(
        "GET /index.html HTTP/1.1\r\n"
        "Host: local"
    );
    BOOST_CHECK(req.hasData());
    BOOST_CHECK(!req.isComplete());
    BOOST_CHECK_EQUAL(req.getMethod(), "GET");
    req.update("host\r\n");
    BOOST_CHECK(req.isComplete());
    BOOST_CHECK_EQUAL(req.getHeader("Host"), "localhost");
    req.clear();
    BOOST_CHECK(!req.hasData());
}

BOOST_AUTO_TEST_CASE(ParseHTTPRequestHeadersFirst)
{
    Request req(
        "GET /index.html HTTP/1.1\r\n"
        "Content-Length: 5\r\n"
        "Host: localhost\r\n\r\n"
    );

    BOOST_CHECK_EQUAL(req.getMethod(), "GET");
    BOOST_CHECK_EQUAL(req.getHeader("Content-Length"), "5");
    BOOST_CHECK_EQUAL(req.getHeader("Host"), "localhost");

    BOOST_CHECK(!req.isComplete());
    req.update("ab");
    BOOST_CHECK(!req.isComplete());
    req.update("cde");
    BOOST_CHECK(req.isComplete());

    BOOST_CHECK_EQUAL(req.getContent(), "abcde");
}

BOOST_AUTO_TEST_CASE(HTTPResponseStatusMessage)
{
    BOOST_CHECK_EQUAL(Response(0).getStatusMessage(), "");
    BOOST_CHECK_EQUAL(Response(105).getStatusMessage(), "Name Not Resolved");
    BOOST_CHECK_EQUAL(Response(200).getStatusMessage(), "OK");
    BOOST_CHECK_EQUAL(Response(400).getStatusMessage(), "Bad Request");
    BOOST_CHECK_EQUAL(Response(404).getStatusMessage(), "Not Found");
    BOOST_CHECK_EQUAL(Response(408).getStatusMessage(), "Request Timeout");
    BOOST_CHECK_EQUAL(Response(500).getStatusMessage(), "Internal Server Error");
    BOOST_CHECK_EQUAL(Response(502).getStatusMessage(), "Not Implemented");
    BOOST_CHECK_EQUAL(Response(504).getStatusMessage(), "Gateway Timeout");
}

BOOST_AUTO_TEST_CASE(WriteHTTPResponse)
{
    Response rsp(200);
    rsp.setHeader("Connection", "close");
    BOOST_CHECK_EQUAL(
        rsp.toString(),
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n\r\n"
    );
}

BOOST_AUTO_TEST_CASE(WriteHTTPResponseWithContent)
{
    Response rsp(200, "Test content.");
    rsp.setHeader("Connection", "close");
    BOOST_CHECK_EQUAL(
        rsp.toString(),
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n\r\n"
        "Test content."
    );
}

BOOST_AUTO_TEST_SUITE_END()
