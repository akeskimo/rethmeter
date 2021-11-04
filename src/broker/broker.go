package main

import (
	"bufio"
	"bytes"
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"net/http"
	"net/url"
)

// Arguments parsed from the command line.
type Arguments struct {
	// Server address that the broker will be listening on.
	serverHost string

	// Server port to accept incoming connections.
	serverPort string

	// Buffer reader maximum size for reading incoming message.
	clientReaderSize int

	// URL where to forward the broker message.
	brokerURL *url.URL
}

func parseArgs() (*Arguments, error) {
	var (
		args   Arguments
		rawURL string
		err    error
	)

	flag.StringVar(&args.serverHost, "host", "localhost", "Server address to bind to.")
	flag.StringVar(&args.serverPort, "port", "4321", "Server port.")
	flag.IntVar(&args.clientReaderSize, "buffer-size", 1024, "Client connection reader size.")
	flag.StringVar(&rawURL, "broker-url", "http://localhost:8080", "Client URL where the broker message will be sent after decoding.")

	flag.Parse()

	args.brokerURL, err = url.Parse(rawURL)
	if err != nil {
		return nil, err
	}

	return &args, nil
}

// sendToClient forwards data to specified URL in request body.
func sendToClient(client *http.Client, url *url.URL, readerSize int, data []byte) error {
	var (
		err      error
		response *http.Response
		values   map[string]string
		payload  []byte
	)

	// Replace dummy data with actual parsed data.
	values = map[string]string{
		"data": string(data),
	}
	payload = make([]byte, readerSize)
	payload, err = json.Marshal(values)
	if err != nil {
		return fmt.Errorf("Marshaling json failed: %s\n", err)
	}

	response, err = client.Post(url.String(), "application/json", bytes.NewBuffer(payload))
	if err != nil {
		return fmt.Errorf("Post failed: %s\n", err)
	}
	defer response.Body.Close()

	if response.StatusCode != http.StatusOK {
		return fmt.Errorf("POST to URL %s failed with status: %s\n", url, response.Status)
	}

	fmt.Printf("POST success: %s\n", url.String())

	return nil
}

// handleConnection reads incoming data from connection and executes
// callback with the received data.
func handleConnection(conn net.Conn, readerSize int, url *url.URL) {
	defer conn.Close()

	fmt.Printf("%s connected.\n", conn.RemoteAddr())

	var (
		bufferedReader *bufio.Reader
		buf            []byte
		nbytes         int
		err            error
		client         *http.Client
	)

	buf = make([]byte, readerSize)

	client = &http.Client{}

	bufferedReader = bufio.NewReaderSize(conn, readerSize)
	for {
		if nbytes, err = bufferedReader.Read(buf); err != nil {
			if err == io.EOF {
				break
			}

			if err != nil {
				fmt.Printf("Reading data failed: %s", err)
				return
			}
		}

		if nbytes == 0 {
			fmt.Printf("No data read. Did the client close connection?\n")
			break
		}
	}

	err = sendToClient(client, url, readerSize, buf)
	if err != nil {
		fmt.Printf("Forwarding message failed: %s\n", err)
	}
}

func main() {
	var (
		err     error
		args    *Arguments
		socket  net.Listener
		address string
	)

	args, err = parseArgs()
	if err != nil {
		log.Fatalf("Parsing arguments failed: %s", err)
	}

	address = net.JoinHostPort(args.serverHost, args.serverPort)

	socket, err = net.Listen("tcp", address)
	if err != nil {
		log.Fatalf("Listening to address %s failed: %s", address, err)
	}
	defer socket.Close()

	for {
		fmt.Printf("Server started listening on: %s\n", address)
		conn, err := socket.Accept()
		if err != nil {
			fmt.Printf("Accepting connection failed: %s", err)
			break
		}

		go handleConnection(conn, args.clientReaderSize, args.brokerURL)
	}
}
