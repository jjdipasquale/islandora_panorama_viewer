			// hide URL field on the iPhone/iPod touch
			function hideUrlBar() {
				
				document.getElementsByTagName("body")[0].style.marginTop="1px";
				window.scrollTo(0, 1);
			}
			window.addEventListener("load", hideUrlBar);
			window.addEventListener("resize", hideUrlBar);
			window.addEventListener("orientationchange", hideUrlBar);
