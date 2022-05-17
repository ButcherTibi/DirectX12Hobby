'use strict';
// class MenuItem {
// 	name = "";
// 	// onclick
// 	/** @type {MenuItem[]} */
// 	children = [];
// }
// class MenuComponent {
// 	/** @type {MenuItem[]} */
// 	items = [];
// 	/**
// 	 * 
// 	 * @param {HTMLElement} parent_elem 
// 	 * @param {MenuItem} item 
// 	 */
// 	#render = (parent_elem: HTMLElement, item) => {
// 		let li = document.createElement("li");
// 		li.textContent = item.name;
// 		if ("children" in item) {
// 			let ol = document.createElement("ol");
// 			for (let child of item.children) {
// 				this.#render(ol, child);
// 			}
// 			li.appendChild(ol);
// 		}
// 		parent_elem.appendChild(li);
// 	}
// 	/**
// 	 * 
// 	 * @param {string} element_id 
// 	 */
// 	render = (element_id) => {
// 		let ol = document.createElement("ol");
// 		for (let item of this.items) {
// 			console.log(item.name);
// 			this.#render(ol, item);
// 		}
// 		let root = document.getElementById(element_id);
// 		root.appendChild(ol);
// 	}
// }
function main() {
    // let menu = new MenuComponent();
    // menu.items = [
    // 	{
    // 		name: "Scene",
    // 		children: [
    // 			{
    // 				name: "New"
    // 			},
    // 			{
    // 				name: "Open"
    // 			},
    // 			{
    // 				name: "Open Recent"
    // 			},
    // 			{
    // 				name: "Save"
    // 			},
    // 			{
    // 				name: "Save As"
    // 			},
    // 			{
    // 				name: "Quit"
    // 			}
    // 		]
    // 	},
    // 	{
    // 		name: "Mesh"
    // 	},
    // 	{
    // 		name: "Learn"
    // 	}
    // ];
    // menu.render("main_menu_root");
    console.log("Hello TSC");
}
main();
