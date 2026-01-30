// Heure
const heureElem = document.getElementById("heureJour");
if (heureElem) {
  const pad = (n) => n.toString().padStart(2, "0");
  const now = new Date();
  const heureStr = pad(now.getHours()) + "h" + pad(now.getMinutes());
  heureElem.dateTime = now.toTimeString().slice(0, 5);
  heureElem.textContent = heureStr;
}

// Date
const date = new Date();
const timeElem = document.getElementById("dateJour");
if (timeElem) {
  timeElem.dateTime = date.toISOString().split("T")[0];
  timeElem.textContent = date.toLocaleDateString("fr-FR", {
    weekday: "long",
    year: "numeric",
    month: "long",
    day: "numeric",
  });
}

// Dark mode
function setThemeByHour() {
  const hour = new Date().getHours();
  const root = document.documentElement;
  if (hour >= 8 && hour < 19) {
    root.classList.add("light-mode");
    root.classList.remove("dark-mode");
  } else {
    root.classList.add("dark-mode");
    root.classList.remove("light-mode");
  }
}
setThemeByHour();
