function makeGuess() {
    var guessInput = document.getElementById("guess-input");
    var guess = guessInput.value.toLowerCase();

    // Create a new XMLHttpRequest object
    var xhr = new XMLHttpRequest();

    // Define the request method, URL, and set it to be asynchronous
    xhr.open("POST", "/cgi-bin/hangman.cgi", true);

    // Set the content type for the request
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");

    // Define the callback function to handle the response
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
            // Parse the JSON response from the server
            var response = JSON.parse(xhr.responseText);

            // Update the hangman and word status based on the response
            updateHangman(response.hangman);
            updateWordStatus(response.wordStatus);

            // Display any additional messages
            var messageDiv = document.getElementById("message");
            messageDiv.innerText = response.message;
        }
    };

    // Prepare the data to be sent in the request body
    var data = "guess=" + encodeURIComponent(guess);

    // Send the request with the data
    xhr.send(data);
}

function updateHangman(hangman) {
    var hangmanDiv = document.getElementById("hangman");
    hangmanDiv.innerText = hangman;
}

function updateWordStatus(wordStatus) {
    var wordStatusDiv = document.getElementById("word-status");
    wordStatusDiv.innerText = "Word: " + wordStatus;
}
