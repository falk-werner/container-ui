import { Api } from "./api.js";
import { login } from "./login.js";
import { format_mem, set_text} from "./util.js";

async function fetch_json(url) {
    const response = await fetch(url);
    return await response.json();
}


async function engine_info(api) {
    const data = await api.version();
    set_text("#engine_name", data.Platform.Name);
    set_text("#engine_version", data.Version);
}

async function system_info(api) {
    const data = await api.info();
    set_text("#system_os", data.OperatingSystem);
    set_text("#system_arch", data.Architecture);
    set_text("#system_ncpu", data.NCPU);
    set_text("#system_memory", format_mem(data.MemTotal));
}

async function disk_usage(api) {
    const data = await api.disk_usage();

    set_text("#df_container_count", data.ContainerUsage.TotalCount);
    set_text("#df_container_total", format_mem(data.ContainerUsage.TotalSize));
    set_text("#df_container_reclaimable", format_mem(data.ContainerUsage.Reclaimable));

    set_text("#df_image_count", data.ImageUsage.TotalCount);
    set_text("#df_image_total", format_mem(data.ImageUsage.TotalSize));
    set_text("#df_image_reclaimable", format_mem(data.ImageUsage.Reclaimable));

    set_text("#df_volume_count", data.VolumeUsage.TotalCount);
    set_text("#df_volume_total", format_mem(data.VolumeUsage.TotalSize));
    set_text("#df_volume_reclaimable", format_mem(data.VolumeUsage.Reclaimable));

    set_text("#df_bcache_count", data.BuildCacheUsage.TotalCount);
    set_text("#df_bcache_total", format_mem(data.BuildCacheUsage.TotalSize));
    set_text("#df_bcache_reclaimable", format_mem(data.BuildCacheUsage.Reclaimable));
}


async function containers(api) {
    const data = await api.containers();
    const element = document.querySelector("#containers");

    for(const container of data) {
        const tr = document.createElement("tr");
        element.appendChild(tr);

        const id = document.createElement("td");
        id.textContent = container.Id;
        tr.appendChild(id);

        const names = document.createElement("td");
        names.textContent = container.Names.map((name) => name.substring(1)).join(" ");
        tr.appendChild(names);

        const image = document.createElement("td");
        image.textContent = container.Image;
        tr.appendChild(image);

        const status = document.createElement("td");
        status.textContent = container.Status;
        tr.appendChild(status);

        const ports = document.createElement("td");
        ports.textContent = container.Ports.join(" ");
        tr.appendChild(ports);
    }    
}

async function images(api) {
    const data = await api.images();
    const element = document.querySelector("#images");

    for(const image of data) {
        const tr = document.createElement("tr");
        element.appendChild(tr);

        const id = document.createElement("td");
        id.textContent = image.Id;
        tr.appendChild(id);

        const tags = document.createElement("td");
        tags.textContent = image.RepoTags.join(" ");
        tr.appendChild(tags);

        const created = document.createElement("td");
        created.textContent = new Date(image.Created * 1000).toISOString();
        tr.appendChild(created);

        const size = document.createElement("td");
        size.textContent = format_mem(image.Size);
        tr.appendChild(size);

    }
}

async function volumes(api) {
    const data = await api.volumes();
    const element = document.querySelector("#volumes");

    for(const volume of data.Volumes) {
        const tr = document.createElement("tr");
        element.appendChild(tr);

        const name = document.createElement("td");
        name.textContent = volume.Name;
        tr.appendChild(name);

        const created = document.createElement("td");
        created.textContent = volume.CreatedAt;
        tr.appendChild(created);
    }
}

function activate_entry(name) {
    const menuentries = document.querySelectorAll(".mainmenu li");
    for(const entry of menuentries) {
        entry.classList.remove("active");
    }
    const entry = document.querySelector(`#menuentry_${name}`);
    entry.classList.add("active");

    const pages = document.querySelectorAll(".page");
    for(const page of pages) {
        page.classList.add("hidden");
    }

    const home = document.querySelector(`#${name}`);
    home.classList.remove("hidden");
}

async function activate_home(api)
{
        activate_entry("home");
}

async function activate_containers(api)
{
    activate_entry("containers");
    containers(api);
}

async function activate_images(api)
{
    activate_entry("images");
    images(api);
}

async function activate_volumes(api)
{
    activate_entry("volumes");
    volumes(api);
}

async function activate_system(api)
{
    activate_entry("system");
    engine_info(api);
    system_info(api);
    disk_usage(api);
}

async function startup() {
    const access_token = await login();


    const splash = document.querySelector("#splash");
    splash.classList.add("hidden");

    if (access_token) {
        const main = document.querySelector("#main");
        main.classList.remove("hidden");

        const api = new Api(access_token);
        document.querySelector("#menuentry_home").addEventListener("click", () => {
            activate_home(api);
        });
        document.querySelector("#menuentry_containers").addEventListener("click", () => {
            activate_containers(api);
        });
        document.querySelector("#menuentry_images").addEventListener("click", () => {
            activate_images(api);
        });
        document.querySelector("#menuentry_volumes").addEventListener("click", () => {
            activate_volumes(api);
        });
        document.querySelector("#menuentry_system").addEventListener("click", () => {
            activate_system(api);
        });

        activate_home(api);
    }
    else {
        const login_failed = document.querySelector("#login_failed");
        login_failed.classList.remove("hidden");
    }
}


if (document.readyState === "loading") {
  document.addEventListener("DOMContentLoaded", startup());
} else {
  startup();
}
