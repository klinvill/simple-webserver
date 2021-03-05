# Basic HTTP Webserver

## Background
This repo contains the code for a basic webserver. The webserver is multi-threaded which allows it to handle concurrent
connections. It serves a simple set of content types (html, javascript, css, jpg, gif, etc.). It was written as a
homework assignment for a Network Systems class. It is written in C++ using the C++11 standard.

## Building the code
The project can both be built using cmake (used for development) and directly through make (since the homework
evaluation machines don't have cmake).

### Building using make
To build the executable using make, simply navigate to this directory and run `make`. The build files will be put in
the `out/` directory. A symbolic link will be created in this directory that will allow you to run the webserver. You
can cleanup the generated artifacts by running `make clean`.

## Running the webserver
Once you have built the executable, you can simply run it using the command `./webserver <port>`. This will start a
server listening on the specified port on all interfaces.

The webserver serves files from the `www/` directory relative to its working directory. As such, a request to
`http://localhost:<port>/index.html` would be responded to with the `www/index.html` file.

## Notable limitations
This webserver is very limited since it was built for a specific homework assignment. In particular, it only supports
GET requests for .html, .txt, .png, .gif, .jpg, .css, and .js files.

It only supports POST requests for .html files, and responds to POST requests by returning the html file with the post
data prepended to it in the format: `<h1>POST DATA</h1><pre>${data_in_post_request}</pre>${html_file_contents}`.

The only error message that is ever returned is a 500 Internal Server Error, even if a 404 would be more appropriate
(page not found).

Keep-alive connections are supported, but only with a fixed timeout of 10 seconds.

All headers are ignored except for Content-Type, Content-Length, and Connection.

Only HTTP versions 1.0 and 1.1 are supported.

## Running the tests
There are two types of tests in this project, unit tests and functional tests. The unit tests are written in c using
the Google Test framework and can be built using cmake. The functional tests are written in python using the pytest
framework. They are not currently integrated into the makefile and should be run separately.
