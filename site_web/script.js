     const hamburger = document.getElementById("hamburger");
    const sidebar = document.getElementById("sidebar");

    hamburger.addEventListener("click", () => {
      sidebar.classList.toggle("hidden");
    });

    console.log("fichier js chargé");