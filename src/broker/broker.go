package main

import (
	"bytes"
	"encoding/binary"
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
	// Server address to bind to.
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
	if args.brokerURL == nil {
		return nil, fmt.Errorf("Parsed url is empty")
	}

	return &args, nil
}

type Measurement struct {
	SensorID string `json:"sensor-id"`
	Unit     string `json:"unit"`
	Value    int    `json:"value"`
}

// Message expresses parsed binary buffer in structure object.
type Message struct {
	// Sensor ID unique identifier.
	SensorID [36]byte

	// Measurement unit.
	Unit [4]byte

	// Measurement value.
	Value int32
}

// sendToClient forwards data to specified URL in request body.
func sendToClient(client *http.Client, url *url.URL, readerSize int, message *Message) error {
	var (
		err         error
		response    *http.Response
		measurement *Measurement
		payload     []byte
	)

	measurement = &Measurement{
		SensorID: string(message.SensorID[:]),
		Unit:     string(message.Unit[:]),
		Value:    int(message.Value),
	}

	payload, err = json.Marshal(measurement)
	if err != nil {
		return fmt.Errorf("Marshaling json failed: %s\n", err)
	}

	response, err = client.Post(url.String(), "application/json", bytes.NewBuffer(payload))
	if err != nil {
		return fmt.Errorf("Post failed: %s\n", err)
	}
	defer response.Body.Close()

	if response.StatusCode != http.StatusOK {
		b, err := io.ReadAll(response.Body)
		if err != nil {
			return fmt.Errorf("Error decoding response status from POST to URL %s, status: %s, error: %s\n", url, response.Status, err)
		}
		return fmt.Errorf("POST to URL %s failed with status: %s, message: %s\n", url, response.Status, b)
	}

	fmt.Printf("POST success: %s\n", url.String())

	return nil
}

// handleConnection reads incoming data from connection and executes
// callback with received data.
func handleConnection(conn net.Conn, readerSize int, url *url.URL) {
	defer conn.Close()

	var (
		err     error
		client  *http.Client
		message *Message
	)

	fmt.Printf("%s connected.\n", conn.RemoteAddr())

	client = &http.Client{}

	message = &Message{}
	for {
		err = binary.Read(conn, binary.LittleEndian, message)
		if err != nil {
			if err == io.EOF {
				break
			}
			fmt.Printf("Unpack failed: %s\n", err)
			return
		}
	}

	err = sendToClient(client, url, readerSize, message)
	if err != nil {
		fmt.Printf("Sending message to client failed: %s\n", err)
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
