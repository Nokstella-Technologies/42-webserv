package WebServer

import (
	"errors"
	"net/http"
	"strings"
)

type Location struct {
	configs map[string]string
}

func NewLocation() *Location {
	return &Location{configs: map[string]string{}}
}

func (l *Location) AddNewLine(line string) error {
	split := strings.Split(line, "=")
	if len(split) != 2 {
		return errors.New("invalid arguments")
	}
	l.configs[strings.TrimSpace(split[0])] = strings.Trim(strings.TrimSpace(split[1]), "\"")
	return nil
}

func (l *Location) handler(w http.ResponseWriter, r *http.Request) {
	l.Print()
	if _, e := w.Write([]byte("Hello, World!")); e != nil {
		println(e)
	}
}

func (l *Location) Print() {
	println("configs:")
	for key, value := range l.configs {
		println(key, value)
	}
}
