// Heure / Date
function updateDateTime() {
  const heureElem = document.getElementById("heureJour");
  const timeElem = document.getElementById("dateJour");
  const now = new Date();
  if (heureElem) {
    const pad = (n) => n.toString().padStart(2, "0");
    const heureStr = pad(now.getHours()) + "h" + pad(now.getMinutes());
    heureElem.dateTime = now.toTimeString().slice(0, 5);
    heureElem.textContent = heureStr;
  }
if (timeElem) {
  timeElem.dateTime = now.toISOString().split("T")[0];
  timeElem.textContent = now.toLocaleDateString("fr-FR", {
      weekday: "long",
      year: "numeric",
      month: "long",
      day: "numeric",
    });
  }
}
updateDateTime();
setInterval(updateDateTime, 1000);

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

//chercher la temperature sur socket
function startTemperatureService() {
  const socket = new WebSocket("ws://localhost:8080");

  const tempElem = document.getElementById("temperature");
  const humidityElem = document.getElementById("humidity");
  const unityElem = document.getElementById("unit");
  const dateMaJElem = document.getElementById("dateMaJ");

  socket.onopen = () => {
    console.log("WebSocket co");
  };

  socket.onmessage = (event) => {
    try {
      const data = JSON.parse(event.data);
      if (tempElem) {
        tempElem.textContent = data.temperature;
      }
      if (humidityElem) {
        humidityElem.textContent = data.humidity ?? "--";
      }
      if (unityElem){
        unityElem.textContent = data.unit ;
      }
      if (dateMaJElem) {
        const now = new Date();
        dateMaJElem.textContent = now.toLocaleTimeString("fr-FR");
      }
    } catch (err) {
      console.error("rreur json", err);
    }
  };

  socket.onerror = (err) => {
    console.error("Erreur socket", err);
  };

  socket.onclose = () => {
    console.warn("socket ferme");
  };
}
startTemperatureService();
