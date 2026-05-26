const board = document.getElementById("board");
const statusDiv = document.getElementById("status");
const submitBtn = document.getElementById("submitBtn");
const resetBtn = document.getElementById("resetBtn");

let currentGuess = "";
let currentTiles = [];
let gameOver = false;

/* ============================================================
   Tile state cycle: gray -> yellow -> green -> gray
   ============================================================ */

function nextColor(tile) {
  if (tile.classList.contains("gray")) {
    tile.classList.remove("gray");
    tile.classList.add("yellow");
  } else if (tile.classList.contains("yellow")) {
    tile.classList.remove("yellow");
    tile.classList.add("green");
  } else if (tile.classList.contains("green")) {
    tile.classList.remove("green");
    tile.classList.add("gray");
  } else {
    tile.classList.add("gray");
  }
}

/* ============================================================
   Create a new guess row
   ============================================================ */

function addGuessRow(word) {
  const row = document.createElement("div");
  row.className = "row";

  currentTiles = [];

  for (let i = 0; i < word.length; i++) {
    const tile = document.createElement("div");
    tile.className = "tile gray";
    tile.textContent = word[i];

    tile.addEventListener("click", () => {
      if (gameOver) return;
      nextColor(tile);
    });

    row.appendChild(tile);
    currentTiles.push(tile);
  }

  board.appendChild(row);
  submitBtn.disabled = false;
}

/* ============================================================
   Flip animation sequence
   ============================================================ */

async function flipTilesSequentially() {
  for (let i = 0; i < currentTiles.length; i++) {
    const tile = currentTiles[i];
    tile.classList.add("flip");
    await new Promise(r => setTimeout(r, 250));
    tile.classList.remove("flip");
  }
}

/* ============================================================
   Get feedback string from tiles (GYBBY format)
   ============================================================ */

function collectFeedback() {
  let feedback = "";

  for (const tile of currentTiles) {
    if (tile.classList.contains("green")) feedback += "G";
    else if (tile.classList.contains("yellow")) feedback += "Y";
    else feedback += "B";
  }

  return feedback;
}

/* ============================================================
   Request next recommendation from server
   ============================================================ */

async function getRecommendation() {
  try {
    statusDiv.textContent = "Computing recommendation...";
    submitBtn.disabled = true;

    const res = await fetch("/next");
    const guess = (await res.text()).trim();

    currentGuess = guess;
    addGuessRow(guess);

    statusDiv.textContent = "Click tiles to enter feedback.";

  } catch (err) {
    statusDiv.textContent = "Error contacting server.";
    console.error(err);
  }
}

/* ============================================================
   Submit feedback to server
   ============================================================ */

submitBtn.addEventListener("click", async () => {
  try {
    submitBtn.disabled = true;

    await flipTilesSequentially();

    const feedback = collectFeedback();

    await fetch("/feedback", {
      method: "POST",
      body: `${currentGuess} ${feedback}`,
    });

    // If all green → solved
    if (/^G+$/.test(feedback)) {
      statusDiv.textContent = "Solved! 🎉";
      gameOver = true;
      return;
    }

    await getRecommendation();

  } catch (err) {
    statusDiv.textContent = "Error submitting feedback.";
    console.error(err);
  }
});

/* ============================================================
   Reset game
   ============================================================ */

resetBtn.addEventListener("click", async () => {
  await fetch("/reset", { method: "POST" });

  board.innerHTML = "";
  gameOver = false;
  currentTiles = [];

  await getRecommendation();
});

/* ============================================================
   Start immediately (IMPORTANT — restores old behavior)
   ============================================================ */

getRecommendation();