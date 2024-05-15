package main

import (
	"fmt"
	"os"

	WebServer "llima-ce.42.fr/m/src"
)

func main() {
	var filename string
	if len(os.Args) != 2 {
		filename = "./configuration/server.toml"
	} else {
		filename = os.Args[1]
	}
	fmt.Println("Usin `configuration file:", filename)
	webserver, e := WebServer.NewWebServer(filename)
	if e != nil {
		fmt.Println(e)
	} else {
		webserver.Start()
	}

}
