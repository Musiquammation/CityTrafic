import { direction } from "../direction"

type DirectionInfo = { id: number; name: string };
const DIRECTIONS: DirectionInfo[] = [
    { id: 0, name: 'RIGHT' },
    { id: 1, name: 'UP' },
    { id: 2, name: 'LEFT' },
    { id: 3, name: 'DOWN' }
];



class TurnSelector {
    private parent: HTMLElement;
    private currentData: number = 0;
    private callback: ((data: number | null) => void) | null = null;

    constructor(parent: HTMLElement) {
        this.parent = parent;
        this.initEvents();
    }

    private initEvents() {
        this.parent.querySelector('#btn-cancel')?.addEventListener('click', () => this.close(null));
        this.parent.querySelector('#btn-ok')?.addEventListener('click', () => this.handleConfirm());
        
        // Update exclusion logic when dropdowns change
        this.parent.querySelectorAll('.main-direction').forEach(select => {
            select.addEventListener('change', () => this.refreshUI());
        });
    }

    take(data: number, callback: (data: number | null) => void) {
        this.currentData = data;
        this.callback = callback;
        this.parent.classList.remove("hidden");

        // Initialiser les directions à partir de data
        const activeDirs = DIRECTIONS.filter(d => direction.getSide(data, d.id) !== 0);
        const dir1 = activeDirs[0]?.id ?? 0;
        const dir2 = activeDirs[1]?.id ?? (dir1 === 1 ? 0 : 1);

        this.setupSelects(dir1, dir2);
        this.refreshUI();
    }

    private setupSelects(val1: number, val2: number) {
        const selects = this.parent.querySelectorAll('.main-direction') as NodeListOf<HTMLSelectElement>;
        selects[0].value = val1.toString();
        selects[1].value = val2.toString();
    }

    private refreshUI() {
        const selects = this.parent.querySelectorAll('.main-direction') as NodeListOf<HTMLSelectElement>;
        const v1 = parseInt(selects[0].value);
        const v2 = parseInt(selects[1].value);

        // 1. Gérer l'exclusion dans les dropdowns
        this.updateDropdownOptions(selects[0], v2);
        this.updateDropdownOptions(selects[1], v1);

        // 2. Générer les sous-menus (checkboxes)
        this.renderSubOptions(0, v1);
        this.renderSubOptions(1, v2);
    }

    private updateDropdownOptions(select: HTMLSelectElement, forbidden: number) {
        const current = select.value;
        select.innerHTML = '';
        DIRECTIONS.forEach(d => {
            if (d.id === forbidden) return;
            const opt = new Option(d.name, d.id.toString());
            opt.selected = d.id.toString() === current;
            select.add(opt);
        });
    }

    private renderSubOptions(groupIndex: number, mainDir: number) {
        const container = this.parent.querySelectorAll('.sub-options')[groupIndex];
        container.innerHTML = '';

        // On exclut l'opposé pour les directions relatives (ex: si UP, on propose RIGHT, LEFT, FRONT)
        const relatives = [
            { bit: 1, label: 'FRONT' },
            { bit: 2, label: 'RIGHT' },
            { bit: 4, label: 'LEFT' }
        ];

        const currentValue = direction.getSide(this.currentData, mainDir);

        relatives.forEach(rel => {
            const label = document.createElement('label');
            const isChecked = (currentValue & rel.bit) || (currentValue === 7); // 7 = ALL
            // Note: Le système de bit 1,2,4 correspond à la logique 0-7 demandée
            
            label.innerHTML = `
                <input type="checkbox" data-bit="${rel.bit}" ${isChecked ? 'checked' : ''}>
                ${rel.label}
            `;
            container.appendChild(label);
        });
    }

    private handleConfirm() {
        let newData = this.currentData;
        const groups = this.parent.querySelectorAll('.input-group');

        groups.forEach(group => {
            const dir = parseInt((group.querySelector('.main-direction') as HTMLSelectElement).value);
            const checks = group.querySelectorAll('input[type="checkbox"]:checked');
            
            let val = 0;
            checks.forEach(c => val += parseInt((c as HTMLInputElement).dataset.bit!));
            
            // Si "All" (Front + Left + Right) est coché ou si val arrive à 7
            if (val > 7) val = 7; 

            newData = direction.setSide(newData, dir, val);
        });

        this.close(newData);
    }

    private close(result: number | null) {
        this.parent.classList.add("hidden");
        if (this.callback) this.callback(result);
    }
}


export const turnSelector = new TurnSelector(
	document.getElementById("turnSelector")!
);