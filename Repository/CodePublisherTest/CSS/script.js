/////////////////////////////////////////////////////////////////////
//  Java Script - Java script for show and hide functionality      //
//  ver 1.0                                                        //
//  Language:      JS				                               //
//  Platform:      Asus R558U, Windows 10                     	   //
//  Application:   Java script for show and hide functionality     //
//  Author:        Nitesh Bhutani, Syracuse University     		   //
//                 (860) 999-3391, nibhutan@syr.edu                //
/////////////////////////////////////////////////////////////////////
function toogleDiv(id) {

	var idElement = document.getElementById(id);
	if(idElement.style.display == 'none')
		idElement.style.display = 'block';
	else
		idElement.style.display = 'none';
}
