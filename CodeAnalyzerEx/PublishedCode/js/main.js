function expandCollapse(button) {
    var butn = button;
	var elem = document.getElementById(butn.id+"_div");
	if(butn.textContent == "+"){
	elem.className = "showElem";
	butn.textContent = "-";
	}
	else{
	elem.className = "noShow";
	butn.textContent = "+";
	}
}
