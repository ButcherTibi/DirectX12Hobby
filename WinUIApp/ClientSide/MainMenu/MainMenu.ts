
export class MenuItem {
	name: string = "";
	children?: MenuItem[] = [];
}

export default class MainMenu {
	private items: MenuItem[] = [];
	private static is_mouse_inside: boolean = false;

	
	static showMenu = (e: MouseEvent) => {
		let li_elem: HTMLElement = e.target as HTMLElement;

		let title_menu: HTMLMenuElement = li_elem.children[1] as HTMLMenuElement;
		if (title_menu === undefined) {
			return;
		}

		title_menu.classList.remove("hide");
	}

	static hideMenu = (e: MouseEvent) => {
		let li_elem: HTMLElement = e.target as HTMLElement;

		let title_menu: HTMLMenuElement = li_elem.children[1] as HTMLMenuElement;
		if (title_menu === undefined) {
			return;
		}

		if (title_menu.classList.contains("hide") === false) {
			title_menu.classList.add("hide");
		}

		// setTimeout()
	}

	private _render = (parent_elem: HTMLElement, item: MenuItem, level: number) => {
		let li_elem = document.createElement("li");
		li_elem.onmouseenter = MainMenu.showMenu;
		li_elem.onmouseleave = MainMenu.hideMenu;

		if (level === 0) {
			li_elem.classList.add("title-item");
		}
		else {
			li_elem.classList.add("normal-item");
		}

		let label = document.createElement("label");
		label.textContent = item.name;

		li_elem.appendChild(label);

		if (item.children && item.children.length > 0) {

			let menu_elem = document.createElement("menu");
			menu_elem.classList.add("hide");

			for (let child of item.children) {
				this._render(menu_elem, child, level + 1);
			}

			li_elem.appendChild(menu_elem);
		}

		parent_elem.appendChild(li_elem);
	}

	render = (element_id: string, new_items: MenuItem[]) => {
		this.items = new_items;

		let menu_elem = document.createElement("menu");
		menu_elem.classList.add("main-menu");

		for (let item of this.items) {
			this._render(menu_elem, item, 0);
		}

		let root = document.getElementById(element_id) as HTMLElement;
		root.appendChild(menu_elem);
	}
}
