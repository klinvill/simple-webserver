import pytest
import subprocess
import time
from typing import Generator
import requests
import socket

PORT = 8080


@pytest.fixture(scope="module")
def server() -> Generator[subprocess.Popen, None, None]:
    with subprocess.Popen(["./cmake-build-debug/webserver", str(PORT)]) as proc:
        # waits for the webserver process to finish starting, should ideally replace with a startup message to watch for
        time.sleep(1)
        yield proc
        proc.kill()

content_types = {
    "html": "text/html",
    "txt": "text/plain",
    "png": "image/png",
    "gif": "image/gif",
    "jpg": "image/jpg",
    "css": "text/css",
    "js": "application/javascript",
}


def expected_content_type(file: str) -> str:
    return content_types[file.rsplit(".", 1)[1]]


@pytest.mark.usefixtures("server")
class TestGet:
    @pytest.mark.parametrize("file", ["index.html", "jquery-1.4.3.min.js", "css/style.css", "fancybox/blank.gif",
                                      "fancybox/fancy_close.png", "fancybox/fancy_loading.png",
                                      "fancybox/fancy_nav_left.png", "fancybox/fancy_nav_right.png",
                                      "fancybox/fancy_shadow_e.png", "fancybox/fancy_shadow_n.png",
                                      "fancybox/fancy_shadow_ne.png", "fancybox/fancy_shadow_nw.png",
                                      "fancybox/fancy_shadow_s.png", "fancybox/fancy_shadow_se.png",
                                      "fancybox/fancy_shadow_sw.png", "fancybox/fancy_shadow_w.png",
                                      "fancybox/fancy_title_left.png", "fancybox/fancy_title_main.png",
                                      "fancybox/fancy_title_over.png", "fancybox/fancy_title_right.png",
                                      "fancybox/fancybox-x.png", "fancybox/fancybox-y.png", "fancybox/fancybox.png",
                                      "fancybox/jquery.easing-1.3.pack.js", "fancybox/jquery.fancybox-1.3.4.css",
                                      "fancybox/jquery.fancybox-1.3.4.js", "fancybox/jquery.fancybox-1.3.4.pack.js",
                                      "fancybox/jquery.mousewheel-3.0.4.pack.js", "files/text1.txt",
                                      "graphics/arrowdown.gif", "graphics/arrowright.gif", "graphics/arrowup.gif",
                                      "graphics/changes.gif", "graphics/doc.gif", "graphics/edittopic.gif",
                                      "graphics/else.gif", "graphics/filter.gif", "graphics/gif.gif",
                                      "graphics/group.gif", "graphics/help.gif", "graphics/home.gif",
                                      "graphics/html.gif", "graphics/index.gif", "graphics/indexlist.gif",
                                      "graphics/info.gif", "graphics/jpg.gif", "graphics/more-small.gif",
                                      "graphics/mov.gif", "graphics/move.gif", "graphics/newtopic.gif",
                                      "graphics/notify.gif", "graphics/pdf.gif", "graphics/person.gif",
                                      "graphics/png.gif", "graphics/ppt.gif", "graphics/recentchanges.gif",
                                      "graphics/rss-small.gif", "graphics/search-small.gif", "graphics/searchtopic.gif",
                                      "graphics/statistics.gif", "graphics/tablesortdiamond.gif",
                                      "graphics/tablesortdown.gif", "graphics/tip.gif", "graphics/topicbacklinks.gif",
                                      "graphics/topicdiffs.gif", "graphics/trash.gif", "graphics/txt.gif",
                                      "graphics/viewtopic.gif", "graphics/warning.gif", "graphics/web-bg-small.gif",
                                      "graphics/web-bg.gif", "graphics/wrench.gif", "graphics/zip.gif",
                                      "images/apple_ex.png", "images/exam.gif", "images/welcome.png",
                                      "images/wine3.jpg"])
    def test_get(self, file: str):
        with open(f"www/{file}", "rb") as f:
            expected_content = f.read()

        r = requests.get(f"http://localhost:{PORT}/{file}")
        assert r.status_code == 200
        assert r.headers['Content-Type'] == expected_content_type(file)
        assert r.content == expected_content
        assert int(r.headers['Content-Length']) == len(expected_content)

    def test_default_index_html(self):
        expected_file = "index.html"
        with open(f"www/{expected_file}", "rb") as f:
            expected_content = f.read()

        r = requests.get(f"http://localhost:{PORT}/")
        assert r.status_code == 200
        assert r.headers['Content-Type'] == expected_content_type(expected_file)
        assert r.content == expected_content
        assert int(r.headers['Content-Length']) == len(expected_content)

    def test_bad_resource(self):
        r = requests.get(f"http://localhost:{PORT}/nonexistent/resource")
        assert r.status_code == 500
        assert r.reason == "Internal Server Error"


@pytest.mark.usefixtures("server")
class TestPost:
    def test_post_index_html(self):
        file = "index.html"
        with open(f"www/{file}", "rb") as f:
            file_content = f.read()

        post_data = b"This is some test post data...."

        expected_content = b"<h1>POST DATA</h1><pre>" + post_data + b"</pre>" + file_content

        r = requests.post(f"http://localhost:{PORT}/{file}", data=post_data)
        assert r.status_code == 200
        assert r.headers['Content-Type'] == expected_content_type(file)
        assert r.content == expected_content
        assert int(r.headers['Content-Length']) == len(expected_content)

    # same as previous test, but not explicitly specifying index.html
    def test_post_default(self):
        file = "index.html"
        with open(f"www/{file}", "rb") as f:
            file_content = f.read()

        post_data = b"This is some test post data...."

        expected_content = b"<h1>POST DATA</h1><pre>" + post_data + b"</pre>" + file_content

        r = requests.post(f"http://localhost:{PORT}/", data=post_data)
        assert r.status_code == 200
        assert r.headers['Content-Type'] == expected_content_type(file)
        assert r.content == expected_content
        assert int(r.headers['Content-Length']) == len(expected_content)

    def test_post_errors_for_non_html_files(self):
        file = "jquery-1.4.3.min.js"

        post_data = b"This is some test post data...."

        r = requests.post(f"http://localhost:{PORT}/{file}", data=post_data)
        assert r.status_code == 500
        assert r.reason == "Internal Server Error"


@pytest.mark.usefixtures("server")
class TestConcurrentRequests:
    def test_concurrent_requests(self):
        # Request 1, opens TCP connection and waits
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(("localhost", PORT))

        file = "index.html"
        with open(f"www/{file}", "rb") as f:
            expected_content = f.read()

        # Request 2, typical get request
        r = requests.get(f"http://localhost:{PORT}/{file}")
        assert r.status_code == 200
        assert r.headers['Content-Type'] == expected_content_type(file)
        assert r.content == expected_content
        assert int(r.headers['Content-Length']) == len(expected_content)

        # Now send Request 1 to the previously opened connection
        s.send(b"FOO / HTTP/1.1 \r\n\r\n")
        tcp_response = s.recv(1024)
        s.close()
        assert tcp_response.startswith(b"HTTP/1.1 500 Internal Server Error\r\n")


@pytest.mark.usefixtures("server")
class TestPipelinedRequests:
    def test_keepalive_persists_by_default(self):
        file = "index.html"
        with open(f"www/{file}", "rb") as f:
            expected_content = f.read()

        with requests.Session() as s:
            r = s.get(f"http://localhost:{PORT}/{file}", headers={"Connection": "keep-alive"})
            assert r.status_code == 200
            assert r.headers['Connection'].lower() == "keep-alive"
            assert r.headers['Content-Type'] == expected_content_type(file)
            assert r.content == expected_content
            assert int(r.headers['Content-Length']) == len(expected_content)

            # omit keep-alive header, but the previous keep-alive should persist
            r = s.get(f"http://localhost:{PORT}/{file}", headers={"Connection": None})
            assert r.status_code == 200
            assert r.headers['Connection'].lower() == "keep-alive"
            assert r.headers['Content-Type'] == expected_content_type(file)
            assert r.content == expected_content
            assert int(r.headers['Content-Length']) == len(expected_content)

    def test_close_connection_if_sent_close(self):
        file = "index.html"
        with open(f"www/{file}", "rb") as f:
            expected_content = f.read()

        with requests.Session() as s:
            r = s.get(f"http://localhost:{PORT}/{file}", headers={"Connection": "keep-alive"})
            assert r.status_code == 200
            assert r.headers['Connection'].lower() == "keep-alive"
            assert r.headers['Content-Type'] == expected_content_type(file)
            assert r.content == expected_content
            assert int(r.headers['Content-Length']) == len(expected_content)

            r = s.get(f"http://localhost:{PORT}/{file}", headers={"Connection": "close"})
            assert r.status_code == 200
            assert r.headers['Connection'].lower() == "close"
            assert r.headers['Content-Type'] == expected_content_type(file)
            assert r.content == expected_content
            assert int(r.headers['Content-Length']) == len(expected_content)

    def test_close_connection_if_no_keepalive(self):
        file = "index.html"
        with open(f"www/{file}", "rb") as f:
            expected_content = f.read()

        with requests.Session() as s:
            r = s.get(f"http://localhost:{PORT}/{file}", headers={"Connection": None})
            assert r.status_code == 200
            assert r.headers['Connection'].lower() == "close"
            assert r.headers['Content-Type'] == expected_content_type(file)
            assert r.content == expected_content
            assert int(r.headers['Content-Length']) == len(expected_content)

    def test_connection_closes_after_timeout(self):
        timeout = 10    # in seconds
        file = "index.html"
        with open(f"www/{file}", "rb") as f:
            expected_content = f.read()

        with requests.Session() as s:
            r = s.get(f"http://localhost:{PORT}/{file}", headers={"Connection": "keep-alive"})
            assert r.status_code == 200
            assert r.headers['Connection'].lower() == "keep-alive"
            assert r.headers['Content-Type'] == expected_content_type(file)
            assert r.content == expected_content
            assert int(r.headers['Content-Length']) == len(expected_content)

            time.sleep(timeout+1)

            # omit keep-alive header, but the previous keep-alive should persist
            r = s.get(f"http://localhost:{PORT}/{file}", headers={"Connection": None})
            assert r.status_code == 200
            assert r.headers['Connection'].lower() == "close"
            assert r.headers['Content-Type'] == expected_content_type(file)
            assert r.content == expected_content
            assert int(r.headers['Content-Length']) == len(expected_content)
