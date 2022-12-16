// ############################# MODAL #############################

if (window.location.href == 'http://localhost:3000/') {

// Get modal element 

var modal = document.getElementById('simpleModal'); 

// Get open modal button

var modalBtn = document.getElementById('modalBtn');

// Get close button

var closeBtn = document.getElementsByClassName('closeBtn')[0];

// Listen for open click

modalBtn.addEventListener('click', openModal);

// Listen for close click

closeBtn.addEventListener('click', closeModal);

// Listen for outside click

window.addEventListener('click', outsideClick);

// Function open modal

function openModal() {
    modal.style.display = 'grid';
}

// Function close modal

function closeModal() {
    modal.style.display = 'none';
}

// Function close modal if outside click

function outsideClick(e) {
    if(e.target == modal) {
        modal.style.display = 'none';
    }
}

};

// ############################# CARROSSEL #############################

const buttons = document.querySelectorAll("[data-carousel-button]");
console.log(buttons);
let previous; 
let next; 

buttons.forEach(button => {
  button.addEventListener("click", () => {
    const offset = button.dataset.carouselButton === "next" ? 1 : -1
    const slides = button
      .closest("[data-carousel]")
      .querySelector("[data-slides]")

    const activeSlide = slides.querySelector("[data-active]")
    let newIndex = [...slides.children].indexOf(activeSlide) + offset
    if (newIndex < 0) newIndex = slides.children.length - 1
    if (newIndex >= slides.children.length) newIndex = 0

    slides.children[newIndex].dataset.active = true
    delete activeSlide.dataset.active

    if (newIndex > 0) {
      previous = newIndex - 1; 
    } else {
      previous = slides.children.length - 1;
    }

    if (newIndex < slides.children.length - 1) {
      next = newIndex + 1;
    } else {
      next = 0;
    }

    let previousCard = slides.children[previous];	
    let nextCard = slides.children[next];
    let currentCard = slides.children[newIndex];

    slides.replaceChildren(previousCard, currentCard, nextCard);

    console.log(previous);
    console.log(newIndex);
    console.log(next);
    
  })
})

function submitForm() {
  var select = document.getElementsByName("areaID")[0];
  var selectedOption = select.options[select.selectedIndex].value;
  console.log(selectedOption); // log the value of the selected option to the console
  document.getElementById("area-select").submit();
};

function submitForm2() {
  var select = document.getElementsByName("userID")[0];
  var selectedOption = select.options[select.selectedIndex].value;
  console.log(selectedOption); // log the value of the selected option to the console
  document.getElementById("area-select").submit();
};

function submitForm3() {
  var select = document.getElementsByName("date")[0];
  var selectedOption = select.options[select.selectedIndex].value;
  console.log(selectedOption); // log the value of the selected option to the console
  document.getElementById("area-select2").submit();
};

