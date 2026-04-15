import { Worker } from "worker_threads";
import os from "os";
import { generateHash } from "./generateHash";
import { Match } from "./Match";



type Pending = {
	resolve: (value: any) => void;
	reject: (reason?: any) => void;
};

class Shared {
	matchs = new Map<string, Match>();

	private cpus: number;
	private workers: Worker[];
	private nextWorker = 0;

	private pending = new Map<number, Pending>();
	private nextRequestId = 0;

	constructor(cpus: number) {
		this.cpus = cpus;
		this.workers = new Array<Worker>(cpus);
	}

	private ask<T=any>(workerId: number, method: string, args: any) {
		const worker = this.workers[workerId % this.cpus];
		const requestId = this.nextRequestId++;
		const promise = new Promise((resolve, reject) => {
			this.pending.set(requestId, { resolve, reject });
		});

		worker.postMessage({
			requestId,
			method,
			args,
		});

		return promise as Promise<T>;
	}


	init() {
		const workerPath = import.meta.resolve('./servWorker.js');
		for (let i = 0; i < this.cpus; i++) {
			const data = { workerData: { indexStart: i, indexSpacing: this.cpus } };

			const imp = `import('tsx/esm/api').then(({ register }) => {
				register(); import('${workerPath}') })`;

			const worker = import.meta.filename.endsWith('.ts')
				? new Worker(imp, { eval: true, ...data })
				: new Worker(workerPath, data);

			worker.on("message", (msg) => {
				this.handleWorkerMessage(msg);
			});

			this.workers[i] = worker;
		}
	}

	private handleWorkerMessage(msg: any) {
		const { requestId, result, error } = msg;

		const pending = this.pending.get(requestId);
		if (!pending) return;

		this.pending.delete(requestId);

		if (error) pending.reject(error);
		else pending.resolve(result);
	}

	async createMatch() {
		const hash = generateHash();

		const workerId = this.nextWorker;
		this.nextWorker = (this.nextWorker + 1) % this.cpus;


		const {id, mapX, mapY, mapW, mapH} = await this.ask<{
			id: number, 
			mapX: number, 
			mapY: number, 
			mapW: number, 
			mapH: number
		}>(
			workerId,
			'createMatch',
			[],
		);

		const match = new Match(id, mapX, mapY, mapW, mapH);

		this.matchs.set(hash, match);

		return {match, hash};
	}


	async collectArea(id: number, x: number, y: number, w: number, h: number) {
		const area = await this.ask<Uint16Array>(
			id,
			'collectArea',
			[id, x,y,w,h],
		);

		return area;
	}

	async placeSingleRoad(id: number, x: number, y: number) {
		await this.ask(
			id,
			'placeSingleRoad',
			[id, x,y],
		);


	}
}

export const shared = new Shared(
	Math.max(1, os.cpus().length - 1)
);

shared.init();
