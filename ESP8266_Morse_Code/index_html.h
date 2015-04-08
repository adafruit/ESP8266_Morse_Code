// Just save the contents of the main HTML page as a string in flash memory.
// Remember this can't insanely big, there's only 512KB total flash!

// Note the raw string literal R syntax is C++11 only!
const char* index_html = R"(
  <!doctype html>
  <html>
    <head>
      <title>Morse Code Blinker</title>
    </head>
    <body>
      <h1>Morse Code Blinker</h1>
      <form action="/morse" method="post">
        <div>
          <label for="message">Message:</label>
          <input type="text" id="message" name="message">
        </div>
        <div class="button">
          <button type="submit">Blink!</button>
        </div>
      </form>
    </body>
  </html>
)";
